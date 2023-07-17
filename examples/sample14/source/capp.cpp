#include "stdafx.h"
#include "capp.h"

//
//	����搶�́u�}���`�X���b�h�̃f�U�C���p�^�[���v�{��
//	���ǖ{�Ƃ��ēǂނ��ƁI
//

/////////////////////////////////////////////////////////////////
//	sample 1 : single thread execution�p�^�[��

namespace test1 {

struct CCommonResource {
	void test1(int nThreadNo){
		synchronized g(cs);
		//	���ʃ��\�[�X�ւ̃A�N�Z�X
		CDbg().Out("test1 : threadNo = "
			+CStringScanner::NumToString(nThreadNo));
	}
	void test2(int nThreadNo){
		synchronized g(cs);
		//	���ʃ��\�[�X�ւ̃A�N�Z�X
		CDbg().Out("test2 : threadNo = "
			+CStringScanner::NumToString(nThreadNo));
	}
protected:
	CCriticalSection cs;
};

struct CUserThread : public CThread {
	CUserThread(const shared_ptr<CCommonResource>& cr
		,int nThreadNo) : cr_(cr),nThreadNo_(nThreadNo) {}

	virtual void ThreadProc() {
		for(int i=0;i<100;++i){
			cr_->test1(nThreadNo_);
			cr_->test2(nThreadNo_);
		}
	}

protected:
	shared_ptr<CCommonResource> cr_;
	int nThreadNo_;
};

void Sample()
{
	shared_ptr<CCommonResource> cr(new CCommonResource);
	CThreadManager::CreateThread(new CUserThread(cr,0));
	CThreadManager::CreateThread(new CUserThread(cr,1));
	//	2�̃X���b�h����ЂƂ̋��p���\�[�X�ɑ΂��ăA�N�Z�X����
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 2 : immutable�p�^�[����write����̂Ȃ�resource�N���X
//	�T���v���͊���
/////////////////////////////////////////////////////////////////
//	sample 3 : guarded suspension
namespace test3 {

struct CRequest { string strData; };

struct CRequestQueue {
/**
	CClientThread�����putRequest(�L���[�ɐς�)�����B
	CServerThread�����getRequest(�L���[����f�[�^���擾����)�����B
	�����ACServerThread����getRequest���ꂽ�Ƃ��ɃL���[�Ƀf�[�^��
	���݂��Ȃ���΁ACClientThread����putRequest�����̂�҂�
*/
	typedef smart_list_ptr<CRequest> request_queue;
		//�@�@��list<smart_ptr<CRequest> >�̈Ӗ�

	smart_ptr<CRequest> getRequest(){
		synchronized g(&m_lock);
		while (m_queue.size() ==0){
			m_lock.wait();	//	�f�[�^���܂����݂��Ȃ��̂ő҂�
		}
		smart_ptr<CRequest> req = *m_queue.begin();
		m_queue.pop_front();
		return req;
	}
	void putRequest(const smart_ptr<CRequest>& req){
		synchronized g(&m_lock);
		m_queue.push_back(req);
		m_lock.notifyAll();
		//	���̃L���[�ɑ΂���push_back��҂��Ă�������
		//	�m��Ȃ����ׂẴX���b�h�ɒʒm����
	}

protected:
	CLockObject m_lock;
	request_queue m_queue;
};

struct CClientThread : public CThread {
	CClientThread(const shared_ptr<CRequestQueue>& rq):rq_(rq){}

	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CRequest> req(new CRequest);
			req->strData = CStringScanner::NumToString(i);
			getRequestQueue()->putRequest(req);
			IThread::getThread()->sleep(2000);
			//	2�b���҂Ƃ��悤
		}
	}
protected:
	shared_ptr<CRequestQueue> getRequestQueue() { return rq_;}
	shared_ptr<CRequestQueue> rq_;
};

struct CServerThread : public CThread {
	CServerThread(const shared_ptr<CRequestQueue>& rq):rq_(rq){}
	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CRequest> req = getRequestQueue()->getRequest();
			//	���N�G�X�g�L���[����f�[�^���擾

			CDbg().Out(req->strData);

			//	���ɂ�����wait����ꂽ�Ƃ��Ă��ARequestQueue��
			//	�o�b�t�@�����O���Ă����̂ň��S�Ɏ��o����
			//	IThread::getThread()->sleep(4000);
		}
	}
protected:
	shared_ptr<CRequestQueue> getRequestQueue() { return rq_;}
	shared_ptr<CRequestQueue> rq_;
};

void Sample()
{
	shared_ptr<CRequestQueue> rq(new CRequestQueue);
	CThreadManager::CreateThread(new CClientThread(rq));
	CThreadManager::CreateThread(new CServerThread(rq));
	//	2�̃X���b�h����ЂƂ̃��N�G�X�g�L���[�ɑ΂��ăA�N�Z�X����
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 4 : balking
//	balk�͏����������A��p�^�[���B�T���v�������B
/////////////////////////////////////////////////////////////////
//	sample 5 : producer-consumer
//		guarded suspention �p�^�[���́Arequestqueue��queue�T�C�Y��
//		������������̂ƍl����Ƃ킩��₷���B
//
//		�C�x���g�f�B�X�p�b�`���O�ɂ�
//		producer�����������āAconsumer���P��Ƃ������f�����g�����Ƃ�����
//		�i�������邱�Ƃɂ��Aconsumer�̓V���O���X���b�h�������\�ɂȂ�j

namespace test5 {

struct CData { string strData; };

struct CChannel {
/**
	CProducerThread�����putData(�f�[�^��u��)�����B
		�u���ꂽ�f�[�^���Aqueue�\���ŊǗ����Ă��邩�ǂ����͎����ɂ��B
	CConsumerThread�����getData(�f�[�^�����o��)�����B

	�����ACConsumerThread����getData���ꂽ�Ƃ��Ƀf�[�^��
	���݂��Ȃ���΁ACProducerThread����putData�����̂�҂�
*/
	typedef smart_list_ptr<CData> data_queue;
		//�@�@��list<smart_ptr<CRequest> >�̈Ӗ�
	enum { bufferlength = 3 };

	smart_ptr<CData> getData(){
		synchronized g(&m_lock);
		while (m_queue.size() ==0){
			m_lock.wait();	//	�f�[�^���܂����݂��Ȃ��̂ő҂�
		}
		smart_ptr<CData> data = *m_queue.begin();
		m_queue.pop_front();

		//	���o�����̂Œʒm���Ă���
		m_lock.notifyAll();
		return data;
	}
	void putData(const smart_ptr<CData>& req){
		synchronized g(&m_lock);
		while (m_queue.size() >= bufferlength){
			m_lock.wait();
			//	�f�[�^�̒u���ꏊ�������ς��Ȃ̂ŋ󂫂��o����̂�҂�
		}

		m_queue.push_back(req);
		m_lock.notifyAll();
		//	���̃L���[�ɑ΂���push_back��҂��Ă�������
		//	�m��Ȃ����ׂẴX���b�h�ɒʒm����
	}

protected:
	CLockObject m_lock;
	data_queue m_queue;
};

struct CProducerThread : public CThread {
	CProducerThread(const shared_ptr<CChannel>& cn,
		const string& name):cn_(cn),name_(name){}

	virtual void ThreadProc() {
		for(int i=0;i<10;++i){
			smart_ptr<CData> data(new CData);
			CDbg().Out(name_+"���o�������I");
			data->strData = name_;
			getChannel()->putData(data);

			IThread::getThread()->sleep(1000);	//	1�b���҂Ƃ��悤
		}
	}
protected:
	shared_ptr<CChannel> getChannel() { return cn_;}
	shared_ptr<CChannel> cn_;
	string name_;
};

struct CConsumerThread : public CThread {
	CConsumerThread(const shared_ptr<CChannel>& cn):cn_(cn){}
	virtual void ThreadProc() {
		for(int i=0;i<20;++i){
			smart_ptr<CData> data = getChannel()->getData();
			//	channel(�����)����f�[�^���擾

			CDbg().Out("����Ȃ̐H���˂���I�R(`�D�L)�m��"+data->strData);

			//	���ɂ�����wait����ꂽ�Ƃ��Ă��AChannel����
			//	�o�b�t�@�����O���Ă����̂ň��S�Ɏ��o����

			// IThread::getThread()->sleep(4000);
		}
	}
protected:
	shared_ptr<CChannel> getChannel() { return cn_;}
	shared_ptr<CChannel> cn_;
};

void Sample()
{
	shared_ptr<CChannel> rq(new CChannel);
	CThreadManager::CreateThread(new CConsumerThread(rq));
	CThreadManager::CreateThread(new CProducerThread(rq,"�E�l�J���["));
	CThreadManager::CreateThread(new CProducerThread(rq,"���}�Y�C���X�^���g���[����"));
	//	2��producer�X���b�h����
	//	�ЂƂ̃��N�G�X�g�L���[�ɑ΂��ăA�N�Z�X����
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 6 : read-write lock
//	read-read�ł͏Փ˂��Ȃ����Ƃ𗘗p���āA
//	write����Ƃ�����lock����B�ǂݍ��݂ɑ΂��Ă�lock������
//	�p�t�H�[�}���X���グ��B�ǂݍ��݂��p�ɂɍs�Ȃ���Ƃ��́A
//	�ǂݍ��݂ɑ΂���Փ˂������Ȃ��̂Ńp�t�H�[�}���X��������

//	����搶�̃}���`�X���b�h�{�������Ă����
//	����p.187�̃T���v���Ɣ�r����
namespace test6 {

struct CReadWriteLock {
//	read-write lock��񋟂���N���X�B���̃N���X���͔̂ėp��������̂�
//	���̂܂ܗ��p�ł��邾�낤
	CReadWriteLock():
		readingReaders(0),waitingWriters(0),
		writingWriters(0),preferWriter(true){}

	void readLock() {
		synchronized guard(getLockObject());
		while (writingWriters > 0 || (preferWriter && waitingWriters>0)){
			getLockObject()->wait();
		}
		readingReaders++;
	}

	void readUnlock() {
		synchronized guard(getLockObject());
		readingReaders--;
		preferWriter = true;
		getLockObject()->notifyAll();
	}

	void writeLock() {
		synchronized guard(getLockObject());
		waitingWriters++;
		try {
			while (readingReaders > 0 || writingWriters >0){
				getLockObject()->wait();
			}
		} catch (...){
		//	wait()�ŃC���^���v�g��O�����������Ƃ���waitingWriters��
		//	�f�N�������g����K�v������B
		//	finally���~�����񂾂�..
				waitingWriters--;
				throw ;
		}
		waitingWriters--;
		writingWriters++;
	}
	void writeUnlock(){
		synchronized guard(getLockObject());
		writingWriters--;
		preferWriter = false;
		getLockObject()->notifyAll();
	}

protected:
	int readingReaders;	//	�ǂݍ��ݒ��̃X���b�h�̐�
	int waitingWriters;	//	�����̂�҂��Ă���X���b�h�̐�
	int writingWriters;	//	�������ݒ��̃X���b�h�̐�
	bool preferWriter;	//	�������݂�D�悷��Ȃ��true
		//	(�X���b�h�̐������������Ȃ����߂̍H�v)

	CLockObject* getLockObject(){ return &lockobject;}
	CLockObject lockobject;
};

struct CCommonResource {
	string read(){
		//	�ǂݍ��݂Ɏ��Ԃ������
		::Sleep(500);
		return str_;
	}
	void write(const string& str){
		//	�������݂ɂ����Ԃ������
		::Sleep(3000);
		str_ = str;
	}

protected:
	string str_;
};

struct CReaderThread : public CThread {
	CReaderThread(const shared_ptr<CReadWriteLock>& rwlock,
		const shared_ptr<CCommonResource>& cr,const string&threadname):
			rwlock_(rwlock),cr_(cr),threadname_(threadname){}

	virtual void ThreadProc() {
		for(int i=0;i<30;++i){
			
			string str;
			getLock()->readLock();
				CDbg().Out(threadname_+"�ǂݍ��ݒ�");
				str = getResource()->read();
				//	�ǂݍ��݂̑O���readLock�`readUnlock���K�v
				CDbg().Out(threadname_+":�ǂݍ��ݕ�����"+str);
			getLock()->readUnlock();

			//	���ɂ�����wait����ꂽ�Ƃ��Ă��AChannel����
			//	�o�b�t�@�����O���Ă����̂ň��S�Ɏ��o����
			//	::Sleep(4000);
		}
	}
protected:
	shared_ptr<CReadWriteLock> getLock() { return rwlock_; }
	shared_ptr<CCommonResource> getResource() { return cr_; }
	string threadname_;

	shared_ptr<CReadWriteLock> rwlock_;
	shared_ptr<CCommonResource> cr_;
};

struct CWriterThread : public CThread {
	CWriterThread(const shared_ptr<CReadWriteLock>& rwlock,
		const shared_ptr<CCommonResource>& cr):rwlock_(rwlock),cr_(cr){}

	virtual void ThreadProc() {
		for(int i=0;i<5;++i){
			
			string str;
			getLock()->writeLock();
				CDbg().Out("�������ݒ�");
				if (i&1) {
					getResource()->write("�那���A�������������݂�`��");
				} else {
					getResource()->write("���C�z�E����A���т΂т΁`�I");
				}
				//	�������݂̑O���writeLock�`writeUnlock���K�v
			getLock()->writeUnlock();

			::Sleep(3000);	//	3�b��1�񏑂�����
		}
	}
protected:
	shared_ptr<CReadWriteLock> getLock() { return rwlock_; }
	shared_ptr<CCommonResource> getResource() { return cr_; }

	shared_ptr<CReadWriteLock> rwlock_;
	shared_ptr<CCommonResource> cr_;
};

void Sample()
{
	shared_ptr<CReadWriteLock> rwlock(new CReadWriteLock);
	shared_ptr<CCommonResource> cr(new CCommonResource);
	CThreadManager::CreateThread(new CWriterThread(rwlock,cr));
	//	�ǂݍ��݃X���b�h��3�p�ӂ���
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"����݇d"));
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"���Ȃ�"));
	CThreadManager::CreateThread(new CReaderThread(rwlock,cr,"���"));
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 7 : thread-per-message
//	���b�Z�[�W(�v��)���ƂɃX���b�h���ЂƂ���Ă����ɈϏ�����
//	��������������
namespace test7 {

struct CHelper : public CThread {
	CHelper(const string& str) : str_(str) {}
	virtual void ThreadProc() {
		Slowly(); // �����������Ɏ��Ԃ̂�����֐����Ǝv���˂�
		CDbg().Out(str_);
	}
	void Slowly() { ::Sleep(3000); }
protected:
	string str_;
};

struct CHost {
	void	doWork(const string& str){
		//	������������邽�߂ɁA�X���b�h���ЂƂ���āA�����ɈϏ�����
		CThreadManager::CreateThread(new CHelper(str));
		//	inner class����`�ł���΂����ɏ����΍ςނ̂���..
	}
};

void Sample()
{
	CHost host;
	host.doWork("�d��������ɂ�I");
	host.doWork("�n�Ԕn�̂悤�ɓ����ɂ�I");
	host.doWork("�����؃p����Ŗ��i����点�ɂ�I");
	CDbg().Out("---�I��---");
	//	���̃T���v�������s����ƁA�I���̕�������ɕ\������A
	//	�d��������ɂ�I�ȉ��̂R�̃��b�Z�[�W�͂R�b��ɕ\�������
	//	����́ACHost::doWork���X���b�h���쐬����(������ɏ������Ϗ�����)
	//	�����𑦍��ɕԂ������Ƃ��Ӗ�����
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 8 : worker thread(thread pool)
//	�X���b�h�����O�ɐ������Ă����A�g���܂킷���ƂŐ����`��̃R�X�g���팸����
//	���̃N���X��CThreadPool�Ƃ���yaneSDK3rd/Thread/yaneThreadPooler.h�ɂ���B
//	�����̂ق��́A��������Q�l�ɂ��Ă������������B

namespace test8 {

void work(int n){
	string id = CStringScanner::NumToString(::GetCurrentThreadId());
	CDbg().Out("1����"+CStringScanner::NumToString(n)
		+"�܂ł̘a�����߂邺:Thread ID="+id);
	int nTotal = 0;
	for(int i=1;i<=n;i++) {
		nTotal+=i;
		::Sleep(10); // ������ƌv�Z����̂Ɏ��Ԃ�������Ƃ��悤
	}
	CDbg().Out("1����"+CStringScanner::NumToString(n)+"�܂ł̘a��"
		+CStringScanner::NumToString(nTotal)+"����:Thread ID="+id);
}

void Sample()
{
	CThreadPooler pool;
	pool.StartThread(3);	// 3�̃X���b�h��ҋ@������
	//				�����̐�����ύX���āA�ǂ��ς�邩�Ⴂ������B
	for(int i=0;i<10;++i){
		smart_ptr<function_callback>
			fn(function_callback_v::Create(&work,i*100+10));
		pool.Invoke(fn);	//	�ҋ@�����Ă���X���b�h�ɏ���������
	}
	pool.wait(); // ���ׂẴX���b�h�̎��s����������̂�҂�
	CDbg().Out("�ЂƎd���I��������Ƃ̃r�[���͂��܂��ɂ偛(���ށ�)o");
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 9 : future
//		�X���b�h�Ɏd����������Ƃ��A���̈������������ɓn���p�^�[��
//		thread-per-message���ł͕K�����̃p�^�[���ň����̎󂯓n�����s�Ȃ�

namespace test9 {

struct CData {	//	���ꂪ�������̖������ʂ���
	void setData(const string& str){
		synchronized guard(lock);
		if (bReady) return ; // balk
		str_ = str;
		bReady = true;
		lock.notifyAll(); // setData�҂��̓z��@���N����
	}
	string getData(){
		synchronized guard(lock);
		while (!bReady){
			lock.wait();
			// �f�[�^���p�ӂ���Ă��Ȃ��̂�
			//	setData�����̂�҂�
		}
		return str_;
	}

	CData():bReady(false){}
protected:
	bool bReady;
	string str_; // ���ꂪ�ێ�����Ă���f�[�^���Ǝv���˂�

	CLockObject lock;
};

//	�����ȉ��́Athread-per-message�p�^�[���Ɠ���
struct CHelper : public CThread {
	CHelper(const string& str,shared_ptr<CData>& data)
		: str_(str),data_(data) {}
	virtual void ThreadProc() {
		Slowly(); // �����������Ɏ��Ԃ̂�����֐����Ǝv���˂�
		data_->setData("�f�[�^���p�ӂł����[�R�i�L�[�M�j�m");
		//	���̕������ݒ肵���u�ԁA���C���X���b�h��getData�ɐ�������

		IThread::getThread()->sleep(2000); // 2�b�҂�
		CDbg().Out(str_); // �u�d��������ɂ�v���\�������
	}
	void Slowly() { ::Sleep(3000); }
protected:
	shared_ptr<CData> data_;
	string str_;
};

struct CHost {
	void	doWork(const string& str,shared_ptr<CData>& data){
		//	������������邽�߂ɁA�X���b�h���ЂƂ���āA�����ɈϏ�����
		CThreadManager::CreateThread(new CHelper(str,data));
		//	inner class����`�ł���΂����ɏ����΍ςނ̂���..
	}
};

void Sample()
{
	CHost host;
	shared_ptr<CData> data(new CData); // �����Ƀf�[�^�������Ă���
	host.doWork("�d��������ɂ�I",data);
	CDbg().Out("--�����͂����A��--");

	string str = data->getData();
	//	getData����ƁA���̃f�[�^���T�u�X���b�h�ɂ����
	//	�p�ӂ����܂ő҂������B

	CDbg().Out("�d�����ʁ�"+str);
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 9a:
//	future�p�^�[����CData�������ł͂Ȃ��Ԃ��l�ɂ���ꍇ�B
//	(CHost::doWork�֐����Asample9�Ɣ�r����)

namespace test9a {

struct CData {	//	���ꂪ�������̖������ʂ���
	void setData(const string& str){
		synchronized guard(lock);
		if (bReady) return ; // balk
		str_ = str;
		bReady = true;
		lock.notifyAll(); // setData�҂��̓z��@���N����
	}
	string getData(){
		synchronized guard(lock);
		while (!bReady){
			lock.wait();
			// �f�[�^���p�ӂ���Ă��Ȃ��̂�
			//	setData�����̂�҂�
		}
		return str_;
	}

	CData():bReady(false){}
protected:
	bool bReady;
	string str_; // ���ꂪ�ێ�����Ă���f�[�^���Ǝv���˂�

	CLockObject lock;
};

//	�����ȉ��́Athread-per-message�p�^�[���Ɠ���
struct CHelper : public CThread {
	CHelper(const string& str,shared_ptr<CData>& data)
		: str_(str),data_(data) {}
	virtual void ThreadProc() {
		Slowly(); // �����������Ɏ��Ԃ̂�����֐����Ǝv���˂�
		data_->setData("�f�[�^���p�ӂł����[�R�i�L�[�M�j�m");
		//	���̕������ݒ肵���u�ԁA���C���X���b�h��getData�ɐ�������

		IThread::getThread()->sleep(2000); // 2�b�҂�
		CDbg().Out(str_); // �u�d��������ɂ�v���\�������
	}
	void Slowly() { ::Sleep(3000); }
protected:
	shared_ptr<CData> data_;
	string str_;
};

struct CHost {
	shared_ptr<CData> doWork(const string& str){
		//	������������邽�߂ɁA�X���b�h���ЂƂ���āA�����ɈϏ�����
		shared_ptr<CData> data(new CData);
		CThreadManager::CreateThread(new CHelper(str,data));
		//	inner class����`�ł���΂����ɏ����΍ςނ̂���..
		return data;
	}
};

void Sample()
{
	CHost host;
	shared_ptr<CData> data = host.doWork("�d��������ɂ�I");
	CDbg().Out("--�����͂����A��--");

	string str = data->getData();
	//	getData����ƁA���̃f�[�^���T�u�X���b�h�ɂ����
	//	�p�ӂ����܂ő҂������B

	CDbg().Out("�d�����ʁ�"+str);
}

} // end of namespace
/////////////////////////////////////////////////////////////////
//	sample 10 : two phase termination
//	�I��������2�i�K�ōs�Ȃ������Ƃ��Ɏg��

namespace test10 {

struct CCountUpThread : public CThread {
	CCountUpThread():nCount(0),bShutDownInterrupt(false){}

	virtual void ThreadProc() {
		try { 
			try {
				while (!bShutDownInterrupt){
					CDbg().Out(++nCount);
					sleep(5000);	//	����sleep���Ɋ��荞�ݗ�O����������
				}
			} catch(CInterruptedException&){
				CDbg().Out("--sleep���Ɋ��荞�݂���������--");
			}
		} catch (...) {		//	finally���~����..
			terminate(); throw ;
		}
		terminate();
	}

	void terminate(){	//	�I�������p�̊֐�
		CDbg().Out("�I���̂��Ƃ����Â����イ");
		::Sleep(3000);
		CDbg().Out("�I���̂��Ƃ����Â�����!!");
	}

	void shutDownRequest(){
		//	�V���b�g�_�E���v���ɂ��X���b�h��invalidate�����
		bShutDownInterrupt=true;
		interrupt();	//	CThread::sleep�ő҂��Ă���A�������N����
	}

protected:
	bool	bShutDownInterrupt;
	int		nCount;
};

void Sample()
{
	CCountUpThread* pThread = new CCountUpThread;
	pThread->CreateThread();
	::Sleep(1000);	//	1�b�҂�
	CDbg().Out("�I���v�����o���܂�");
	pThread->shutDownRequest(); // �V���b�g�_�E����v��
	CDbg().Out("�X���b�h�̏I����҂��܂�");
	pThread->StopThread(); // �X���b�h�̒�~��҂�
	CDbg().Out("�X���b�h�I��");
}

} // end of namespace

/////////////////////////////////////////////////////////////////
//	sample 11 : thread-specific storage
//	yaneSDK3rd��ThreadLocal�Ȃ̂Ŏg�p��͊���
/////////////////////////////////////////////////////////////////
//	sample 12 : active object(actor)
//	�񓯊��ȗv�����󗝂���\���I�I�u�W�F�N�g
//	�o�b�N�O���E���h��worker thread��1�����Ă��āA
//	�������������s�Ȃ�

namespace test12 {

//	�X�}�[�g�|�C���^�������������̖ʓ|
#define p(T) shared_ptr<T>

// �����������������̖ʓ|
#define p_(T) const shared_ptr<T>&

//	Java���ɂ��Ă�����
#define Object shared_ptr<void>
#define Object_ const shared_ptr<void>&

struct IResult {
//	future�p�^�[���ŕԂ��l�������������߂̊��N���X
	virtual Object getResultValue()=0;
};

struct CRealResult : public IResult {
//	�Ԃ��l��ێ����邽�߂̃N���X
	CRealResult(Object_ resultValue){
		this->resultValue = resultValue;
	}
    Object getResultValue() {
        return resultValue;
    }
protected:
	Object resultValue;
};

struct CFutureResult : public IResult {
//	future�p�^�[����Result��Ԃ�
	CFutureResult():ready(false){}
    void setResult(p_(IResult) result) {
		synchronized g(&lock);
		this->result = result;
        this->ready = true;
		lock.notifyAll();
    }
	Object getResultValue() {
		synchronized g(&lock);
        while (!ready) {
            try {
                lock.wait();
            } catch (CInterruptedException&) {
            }
        }
        return result->getResultValue();
    }
protected:
	CLockObject lock;
	p(IResult) result;
    bool ready;
};

/*	//	����future�N���X���g�����߂̃T���v��

void test1(){
	Object	obj(new string("���т΂т΁`"));
	p(IResult) real(new CRealResult(obj));

	p(CFutureResult) result(new CFutureResult);

	//	����result�ɑ΂���getResultValue���s�Ȃ���
	//	���̃X���b�h��setResult����܂ő҂������

	//	a.�u�������v�ƌ�������u�f�[�^�v��n��
	result->setResult(real);

	//	b.�u�������v�Ɓu�f�[�^�v����������
	obj = result->getResultValue();

	//	�u�f�[�^�v��unboxing����
	string str = *smart_ptr_static_cast<string>(obj);
	CDbg().Out(str);
}
*/

struct IActiveObject {
//	���̃N���X���Aactive object���N���X
//	���̃N���X�̃��\�b�h�͔񓯊��ɌĂяo�����
	virtual void displayString(const string&) = 0;
	//	�v���P�D

	virtual p(IResult) makeString(int count, char fillchar) = 0;
	//	�v���Q�D���ʂ́Afuture�p�^�[���ł��炤

	virtual void shutdown() =0;
	//	�V���b�g�_�E�����N�G�X�g(���ꂪ������active object���~����)
};

struct CServant : public IActiveObject {
//	����́A�A�N�e�B�u�I�u�W�F�N�g�̎�����
	virtual p(IResult) makeString(int count, char fillchar) {
		smart_ptr<char> buffer;
		buffer.AddArray(count+1);
		buffer[count] = '\0';
        for (int i = 0; i < count; i++) {
            buffer[i] = fillchar;
            try {
				IThread::getThread()->sleep(100);
            } catch (CInterruptedException&) {
            }
        }
		Object obj(new string(buffer.get()));
			//	string��Object�^��boxing

        return p(IResult)(new CRealResult(obj));
    }	
	virtual void displayString(const string&str) {
		try {
			CDbg().Out("displayString: " + str);
//			CThread()->getMyThread()->sleep(10);
        } catch (CInterruptedException &) {
        }
    }
	virtual void shutdown() {}
};

struct IMethodRequest {
//	active object�ɓ�������v����ۑ����Ă������߂̊��N���X
	virtual void execute()=0;
	//	����Ŏ��s����

protected:
	IMethodRequest(p_(CServant) servant,p_(CFutureResult) future) {
        this->servant = servant;
        this->future = future;
    }

	p(CServant) servant;
    p(CFutureResult) future;
};

struct CActivationQueue {
//	�X�P�W���[����ێ����邽�߂̃L���[
	enum { MAX_METHOD_REQUEST = 100 }; // �ő僊�N�G�X�g��

	void putRequest(const shared_ptr<IMethodRequest>& request) {
		synchronized g(&lock);
        while (MAX_METHOD_REQUEST < requestQueue.size()) {
			//	����ȏ�queuing�s�\�Ȃ�d���Ȃ��̂ő҂�
            lock.wait();
        }
		requestQueue.push_back(request);
        lock.notifyAll();
    }
	p(IMethodRequest) takeRequest() {
		synchronized g(&lock);
        while (requestQueue.size() <= 0) {
			//	�d�����Ȃ��Ȃ�d���Ȃ��̂ő҂�
            lock.wait();
        }
        p(IMethodRequest) request = *requestQueue.begin();
		requestQueue.pop_front();
        lock.notifyAll();
        return request;
    }
protected:
	CLockObject lock;
	list<p(IMethodRequest) >	requestQueue;
};

struct CSchedulerThread : public CThread {
//	�X�P�W���[����ێ����Ă������ԂɎ��s����X���b�h

	CSchedulerThread(p_(CActivationQueue) queue) {
        this->queue = queue;
    }
	
	//	�X�P�W���[���[�ɗv����ς�
	void invoke(p_(IMethodRequest) request) {
        queue->putRequest(request);
    }

	virtual void ThreadProc() {
		try {
			while (true) {
				p(IMethodRequest) request = queue->takeRequest();
				request->execute();
				//	�L���[�ɐς܂�Ă�����e���ЂƂ����s���邾���ŗǂ�
			}	//	�I������Ƃ��͂��̃X���b�h�ɑ΂��Ċ��荞�݂�������
		} catch (CInterruptedException&){
			//	shutdown�ɂ����IThread::interrupt���Ăяo����A���̌���
			//	���̃X���b�h��CLockObject::wait���Ă���΂�����interrupt��O��
			//	��������B�i����Ă����ɔ����Ă���j
		}
		CDbg().Out("ActiveObject���I�����܂���");
    }

	virtual void shutdown() {
		interrupt();
		//	���̃X���b�h�ɑ΂���CInterruptedException�𔭐�������
	}
protected:
	p(CActivationQueue) queue;
};

struct CMakeStringRequest : public IMethodRequest {
//	�v��1��IMethodRequest�h���N���X�ɂ�������
    CMakeStringRequest(p_(CServant) servant,p_(CFutureResult) future,
		int count, char fillchar)
		: IMethodRequest(servant, future)
	{
        this->count = count;
        this->fillchar = fillchar;
    }
    void execute() {
        p(IResult) result = servant->makeString(count, fillchar);
        future->setResult(result);
		//	future�p�^�[���Ŏ󂯓n������
    }
protected:
	int count;
    char fillchar;
};

struct CDisplayStringRequest : public IMethodRequest {
	CDisplayStringRequest(p_(CServant) servant,const string& str)
		: IMethodRequest(servant,p(CFutureResult)())
	{
        this->str = str;
    }
    void execute() {
        servant->displayString(str);
    }
protected:
    string str;
};

struct CProxy : public IActiveObject {
    CProxy(p_(CSchedulerThread) scheduler,p_(CServant) servant) {
        this->scheduler = scheduler;
        this->servant = servant;
    }
	virtual p(IResult) makeString(int count, char fillchar) {
		p(CFutureResult) future(new CFutureResult());
        scheduler->invoke(p(IMethodRequest)
			(new CMakeStringRequest(servant, future, count, fillchar))
		);
		//	�X�P�W���[���[�ɂ��肢���āAfuture�p�^�[����
		//	�����������n���Đ���������ɖ߂�
		return smart_ptr_static_cast<IResult>(future);
		// IResult�ɕϊ����Ė߂��K�v����
    }
    virtual void displayString(const string& str) {
        scheduler->invoke(p(IMethodRequest)
			(new CDisplayStringRequest(servant, str))
		);
		//	�X�P�W���[���[�ɂ��肢���Ă����ɐ����߂�
    }
	virtual void shutdown(){
		//	�X�P�W���[���ɃV���b�g�_�E���v��
		scheduler->shutdown();
	}
protected:
	p(CSchedulerThread) scheduler;
	p(CServant) servant;
};

struct CMakerClientThread : public CThread {
	CMakerClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=5;++i){
				p(IResult) result = actor_->makeString(i,'C');
				//	�߂�l����active object�̌Ăяo��
				Object obj = result->getResultValue();
				const string& str = *smart_ptr_static_cast<string>(obj);
				//	unboxing
				//	(Object�^�ɕ������ꂽstring�^�I�u�W�F�N�g�����o��)
				//	string str = **(smart_ptr<string>*)(obj.get());
				//	�ł������̂����R�s�[�R�X�g�����������Ȃ��̂�

				CDbg().Out(name_ + "��" + str + "�������");
				IThread::getThread()->sleep(200);
			}
		} catch(CInterruptedException&){
			//	sleep���Ɋ��荞�ݗ�O����������?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CDisplayClientThread : public CThread {
	CDisplayClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=10;++i){
				string data = name_ + "��"
					+ CStringScanner::NumToString(i) + "��\������";
				actor_->displayString(data);
				IThread::getThread()->sleep(200);
				//	����sleep���Ɋ��荞�ݗ�O����������(����)
			}
		} catch(CInterruptedException&){
			//	sleep���Ɋ��荞�ݗ�O����������?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CActiveObjectFactory {
	static p(IActiveObject) createActiveObject() {
        p(CServant) servant(new CServant());
		//	active object
		
		p(CActivationQueue) queue(new CActivationQueue());
		// �v��queue
        
		p(CSchedulerThread) scheduler(new CSchedulerThread(queue));
		//	�v�������Ԃɕێ����Ă������߂̃X���b�h

		//	proxy�I�u�W�F�N�g���A�X�P�W���[��(��active object)�ɋ��n������
		p(IActiveObject) proxy(new CProxy(scheduler, servant));
		CThreadManager::CreateThread(
			smart_ptr_static_cast<IThread>(scheduler));
		//	��shared_ptr<CSchedulerThread>��shared_ptr<IThread>��upcast����
        return proxy;
    }
};

struct CShutdownRequester : public CThread {
//	10�b���shutdown request���o��
	CShutdownRequester(p_(IActiveObject) actor){
		this->actor = actor;
	}
	virtual void ThreadProc() {
		sleep(10000);
		CDbg().Out("shutdown�v���������܂�");
		actor->shutdown();
	}
protected:
	p(IActiveObject) actor;
};

void Sample()
{
	p(IActiveObject) actor(CActiveObjectFactory::createActiveObject());
	CThreadManager::CreateThread(new CMakerClientThread("�f�W�R",actor));
	CThreadManager::CreateThread(new CMakerClientThread("�V���K�[",actor));
	CThreadManager::CreateThread(new CDisplayClientThread("�Q�}�Q�}",actor));
	CThreadManager::CreateThread(new CShutdownRequester(actor));
}

//	�ς�define�����̂��͂����Ă�����
#undef p
#undef p_
#undef Object
#undef Object_

} // end of namespace test12

/////////////////////////////////////////////////////////////////

namespace test13 {

void Sample(){
	CDbg().Out("Thread making test -- start");
	CTimer t; t.Reset();
	for(int i=0;i<1000;++i){
		CThread th;
		th.CreateThread();
		th.StopThread();
	}
	DWORD d = t.Get();
	CDbg().Out("Thread making test -- end : time ="
		+ CStringScanner::NumToString(d));
	//	�X���b�h�����`���Ńe�X�g�B1��3ms�قǂ������Ƃ��(Pentium4 2.53GHz)
	//	�܂��A�X���b�h�I����҂��ԂƂ������邵��..
}

} // end of namespace test13

/////////////////////////////////////////////////////////////////

//	function_callback��p���ď�����������
namespace test12a {

//	�X�}�[�g�|�C���^�������������̖ʓ|
#define p(T) shared_ptr<T>

// �����������������̖ʓ|
#define p_(T) const shared_ptr<T>&

//	Java���ɂ��Ă�����
#define Object shared_ptr<void>
#define Object_ const shared_ptr<void>&

struct IResult {
//	future�p�^�[���ŕԂ��l�������������߂̊��N���X
	virtual Object getResultValue()=0;
};

struct CRealResult : public IResult {
//	�Ԃ��l��ێ����邽�߂̃N���X
	CRealResult(Object_ resultValue){
		this->resultValue = resultValue;
	}
    Object getResultValue() {
        return resultValue;
    }
protected:
	Object resultValue;
};

struct CFutureResult : public IResult {
//	future�p�^�[����Result��Ԃ�
	CFutureResult():ready(false){}
    void setResult(p_(IResult) result) {
		synchronized g(&lock);
		this->result = result;
        this->ready = true;
		lock.notifyAll();
    }
	Object getResultValue() {
		synchronized g(&lock);
        while (!ready) {
            try {
                lock.wait();
            } catch (CInterruptedException&) {
            }
        }
        return result->getResultValue();
    }
protected:
	CLockObject lock;
	p(IResult) result;
    bool ready;
};

struct IActiveObject {
//	���̃N���X���Aactive object���N���X
//	���̃N���X�̃��\�b�h�͔񓯊��ɌĂяo�����
	virtual void displayString(const string&) = 0;
	//	�v���P�D

	virtual p(IResult) makeString(int count, char fillchar) = 0;
	//	�v���Q�D���ʂ́Afuture�p�^�[���ł��炤

	virtual void shutdown() =0;
	//	�V���b�g�_�E�����N�G�X�g(���ꂪ������active object���~����)
};

struct CServant : public IActiveObject {
//	����́A�A�N�e�B�u�I�u�W�F�N�g�̎�����
	virtual p(IResult) makeString(int count, char fillchar) {
		smart_ptr<char> buffer;
		buffer.AddArray(count+1);
		buffer[count] = '\0';
        for (int i = 0; i < count; i++) {
            buffer[i] = fillchar;
            try {
				IThread::getThread()->sleep(100);
            } catch (CInterruptedException&) {
            }
        }
		Object obj(new string(buffer.get()));
			//	string��Object�^��boxing

        return p(IResult)(new CRealResult(obj));
    }	
	virtual void displayString(const string&str) {
		try {
			CDbg().Out("displayString: " + str);
//			CThread()->getMyThread()->sleep(10);
        } catch (CInterruptedException &) {
        }
    }
	virtual void shutdown() {}
};

typedef function_callback IMethodRequest;

struct CActivationQueue {
//	�X�P�W���[����ێ����邽�߂̃L���[
	enum { MAX_METHOD_REQUEST = 100 }; // �ő僊�N�G�X�g��

	void putRequest(const shared_ptr<IMethodRequest>& request) {
		synchronized g(&lock);
        while (MAX_METHOD_REQUEST < requestQueue.size()) {
			//	����ȏ�queuing�s�\�Ȃ�d���Ȃ��̂ő҂�
            lock.wait();
        }
		requestQueue.push_back(request);
        lock.notifyAll();
    }
	p(IMethodRequest) takeRequest() {
		synchronized g(&lock);
        while (requestQueue.size() <= 0) {
			//	�d�����Ȃ��Ȃ�d���Ȃ��̂ő҂�
            lock.wait();
        }
        p(IMethodRequest) request = *requestQueue.begin();
		requestQueue.pop_front();
        lock.notifyAll();
        return request;
    }
protected:
	CLockObject lock;
	list<p(IMethodRequest) >	requestQueue;
};

struct CSchedulerThread : public CThread {
//	�X�P�W���[����ێ����Ă������ԂɎ��s����X���b�h

	CSchedulerThread(p_(CActivationQueue) queue) {
        this->queue = queue;
    }
	
	//	�X�P�W���[���[�ɗv����ς�
	void invoke(p_(IMethodRequest) request) {
        queue->putRequest(request);
    }

	virtual void ThreadProc() {
		try {
			while (true) {
				p(IMethodRequest) request = queue->takeRequest();
				// request->execute();
				request->run();
				//	�L���[�ɐς܂�Ă�����e���ЂƂ����s���邾���ŗǂ�
			}	//	�I������Ƃ��͂��̃X���b�h�ɑ΂��Ċ��荞�݂�������
		} catch (CInterruptedException&){
			//	shutdown�ɂ����IThread::interrupt���Ăяo����A���̌���
			//	���̃X���b�h��CLockObject::wait���Ă���΂�����interrupt��O��
			//	��������B�i����Ă����ɔ����Ă���j
		}
		CDbg().Out("ActiveObject���I�����܂���");
    }

	virtual void shutdown() {
		interrupt();
		//	���̃X���b�h�ɑ΂���CInterruptedException�𔭐�������
	}
protected:
	p(CActivationQueue) queue;
};

struct CProxy : public IActiveObject {
    CProxy(p_(CSchedulerThread) scheduler,p_(CServant) servant) {
        this->scheduler = scheduler;
        this->servant = servant;
    }
	virtual p(IResult) makeString(int count, char fillchar) {
		p(CFutureResult) future(new CFutureResult());
        scheduler->invoke(p(IMethodRequest)
			(function_callback_v::Create(
				&CProxy::inner_makeString,
				servant,future,count,fillchar)
			)
		);
		//	�X�P�W���[���[�ɂ��肢���āAfuture�p�^�[����
		//	�����������n���Đ���������ɖ߂�
		return smart_ptr_static_cast<IResult>(future);
    }
	//���̊֐��̃R�[���o�b�N����̃W�����v��
	static void inner_makeString(p_(CServant) s,p(CFutureResult)& future
		,int count,char fillchar){
        p(IResult) result = s->makeString(count, fillchar);
        future->setResult(result);
	}

    virtual void displayString(const string& str) {
        scheduler->invoke(p(IMethodRequest)
			(function_callback_v::Create(
				&CProxy::inner_displayString,servant,str)
			)
		);
		//	�X�P�W���[���[�ɂ��肢���Ă����ɐ����߂�
    }
	//	������̃W�����v��
	static void inner_displayString(p_(CServant) s,const string& str){
		s->displayString(str);
	}

	virtual void shutdown(){
		//	�X�P�W���[���ɃV���b�g�_�E���v��
		scheduler->shutdown();
	}
protected:
	p(CSchedulerThread) scheduler;
	p(CServant) servant;
};

struct CMakerClientThread : public CThread {
	CMakerClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=5;++i){
				p(IResult) result = actor_->makeString(i,'C');
				//	�߂�l����active object�̌Ăяo��
				Object obj = result->getResultValue();
				const string& str = *smart_ptr_static_cast<string>(obj);
				//	unboxing
				//	(Object�^�ɕ������ꂽstring�^�I�u�W�F�N�g�����o��)
				//	string str = **(smart_ptr<string>*)(obj.get());
				//	�ł������̂����R�s�[�R�X�g�����������Ȃ��̂�

				CDbg().Out(name_ + "��" + str + "�������");
				IThread::getThread()->sleep(200);
			}
		} catch(CInterruptedException&){
			//	sleep���Ɋ��荞�ݗ�O����������?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CDisplayClientThread : public CThread {
	CDisplayClientThread(const string&name,p_(IActiveObject)actor)
		:name_(name),actor_(actor){}
	virtual void ThreadProc() {
		try { 
			for(int i=1;i<=10;++i){
				string data = name_ + "��"
					+ CStringScanner::NumToString(i) + "��\������";
				actor_->displayString(data);
				IThread::getThread()->sleep(200);
				//	����sleep���Ɋ��荞�ݗ�O����������(����)
			}
		} catch(CInterruptedException&){
			//	sleep���Ɋ��荞�ݗ�O����������?
		}
	}
protected:
	string	name_;
	p(IActiveObject) actor_;
};

struct CActiveObjectFactory {
	static p(IActiveObject) createActiveObject() {
        p(CServant) servant(new CServant());
		//	active object
		
		p(CActivationQueue) queue(new CActivationQueue());
		// �v��queue
        
		p(CSchedulerThread) scheduler(new CSchedulerThread(queue));
		//	�v�������Ԃɕێ����Ă������߂̃X���b�h

		//	proxy�I�u�W�F�N�g���A�X�P�W���[��(��active object)�ɋ��n������
		p(IActiveObject) proxy(new CProxy(scheduler, servant));
		CThreadManager::CreateThread(
			smart_ptr_static_cast<IThread>(scheduler));
		//	��shared_ptr<CSchedulerThread>��shared_ptr<IThread>��upcast����
        return proxy;
    }
};

struct CShutdownRequester : public CThread {
//	10�b���shutdown request���o��
	CShutdownRequester(p_(IActiveObject) actor){
		this->actor = actor;
	}
	virtual void ThreadProc() {
		sleep(10000);
		CDbg().Out("shutdown�v���������܂�");
		actor->shutdown();
	}
protected:
	p(IActiveObject) actor;
};

void Sample()
{
	p(IActiveObject) actor(CActiveObjectFactory::createActiveObject());
	CThreadManager::CreateThread(new CMakerClientThread("�f�W�R",actor));
	CThreadManager::CreateThread(new CMakerClientThread("�V���K�[",actor));
	CThreadManager::CreateThread(new CDisplayClientThread("�Q�}�Q�}",actor));
	CThreadManager::CreateThread(new CShutdownRequester(actor));
}

//	�ς�define�����̂��͂����Ă�����
#undef p
#undef p_
#undef Object
#undef Object_

} // end of namespace test12a

/////////////////////////////////////////////////////////////////////////////

//	���킸�ƒm�ꂽWinMain
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	CAppInitializer init(hInstance,hPrevInstance,lpCmdLine,nCmdShow);	//	�K�������Ă�

	test8::Sample();

//	test12a::Sample();
	//	���Ăяo���T���v���̃i���o�[���w�肵�ă`��
	//	��)test3::Sample();

	return 0;
}
