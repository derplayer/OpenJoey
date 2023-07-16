//
//	�V���A���C�Y�p�N���X
//			programmed by yaneurao	'01/07/16
//
//		�ЂƂ̃V���A���C�Y�֐��ŁA�X�g���[���ւ̕ۑ��^������
//		���p���邠���肪�A�V���Ȕ����̂悤�ȋC������̂����A
//		�����ł������̂��낤���D�D�O�O�G
//

#ifndef __yaneSerialize_h__
#define __yaneSerialize_h__


namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class ISerialize;
class IArchive {
/**
	�V���A���C�Y�p�A�[�J�C�u�N���X

// �V���A���C�Y�p�A�[�J�C�u�N���X

virtual void Serialize(ISerialize&)=0;
// �������I�[�o�[���C�h���Ďg���Ƃ����̂�

��j
�@�@void Serialize(ISerialize&s){
�@�@�@s << nData << strData;
�@�@}

// ����Ȋ����ŃV���A���C�Y�������A�����o�����ׂď����o���R�[�h�������ׂ�

��������΁A
�@�@�@CSerialize s;
�@�@�@CArchive�h���N���X a;
�@�@�@s << a;
�ƃV���A���C�Y�ł���悤�ɂȂ�B

*/
public:
	virtual void Serialize(ISerialize&)=0;
	//	�������I�[�o�[���C�h���Ďg���Ƃ����̂�

	virtual ~IArchive() {} // place holder

	/**
		��j
			void Serialize(CSerialize&s){
				s << nData << strData;
			}
			//	����Ȋ����ŕK�v�ȃ����o�����ׂď����o���R�[�h�������ׂ�
	*/
};

/**
		�ˁ@CSerialize�̃V���A���C�Y�͌��݂̃X�g�A����(IsStoring)
			�Q�ƈʒu(GetPos)���A�ꏏ�ɃV���A���C�Y����̂ŁA
			�������ɂ́A�����𒍈ӂ���K�v������B
*/

class ISerialize {
public:
	virtual	void	Clear()=0;

	virtual bool	IsStoring() const =0;
	virtual bool*	GetStoring()=0;
	virtual void	SetStoring(bool b)=0;
	virtual void	InnerSetStoring(bool b)=0;
	virtual vector<BYTE>* GetData()=0;
	virtual ISerialize& operator << (IArchive& vData)=0;
	virtual ISerialize& operator << (int& nData)=0;
	virtual ISerialize& operator << (bool& bData)=0;
	virtual ISerialize& operator << (BYTE& byData)=0;
	virtual ISerialize& operator << (string& strData)=0;
	virtual ISerialize& operator << (WORD& wData)=0;
	virtual ISerialize& operator << (DWORD& dwData)=0;
	virtual ISerialize& operator << (vector<int>& anData)=0;
	virtual ISerialize& operator << (vector<bool>& abData)=0;
	virtual ISerialize& operator << (vector<BYTE>& abyData)=0;
	virtual ISerialize& operator << (vector<string>& astrData)=0;
	virtual ISerialize& operator << (vector<WORD>& awData)=0;
	virtual ISerialize& operator << (vector<DWORD>& adwData)=0;
	virtual ISerialize& operator << (ISerialize& vData)=0;
	virtual ISerialize& operator = (ISerialize& vSeri)=0;
	virtual LRESULT Save(const string& filename)=0;
	virtual LRESULT Load(const string& filename)=0;
	virtual int*	GetPos()=0;

	///	�C�ӂ̃f�[�^�^�������o�����߂̂���
	///	�����o�֐��e���v���[�g�Ŏ���
	template <class T>
	ISerialize& SerializeT (T& vData){
		if (IsStoring()){
		//	�ۑ�����񂩂��ȁH
			//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				GetData()->push_back(pByte[i]);
			}
		} else {
		//	�X�g���[��������o���񂩂��ȁH
			//	�T�C�Y�s���Ɖ��肵���R�[�f�B���O�O�O�G
			int nSize = sizeof(T);
			BYTE* pByte = (BYTE*)&vData;
			for(int i=0;i<nSize;i++){
				pByte[i] = (*GetData())[(*GetPos())++];
			}
		}
		return *this;
	}

	///	���邢�́AArray���A�A
	///	�����o�֐��e���v���[�g�ł̎���
	template <class T>
	ISerialize& Store(T* pavData,int nSize){
		///	pavData[nSize]���V���A���C�Y
		if (IsStoring()){
			//	�ۑ�����񂩂��ȁH
			//	this << nSize;		//	�T�C�Y���i�[����K�v�͖���
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		} else {
			//	���o���񂩂��ȁH
			//	*this << nSize;		//�@�T�C�Y�͖��m�ɂ킩���Ă���
			for(int i=0;i<nSize;i++){
				*this << *pavData;
				pavData++;
			}
		}
		return *this;
	}

	virtual ~ISerialize(){}
};

class CSerialize : public ISerialize {
/**
	�V���A���C�Y�p�N���X

	�V���A���C�Y�i�f�[�^�i�����j�̂��ߎ�i�B
	�����o�ϐ��E�z��Ȃǂ��A�X�g���[���ɂ����݁A���邢�͎��o���āA
	������t�@�C���ɕۑ��^�������邱�ƂŁA�A�v���P�[�V�������I���������Ƃ��A
	�O��̏�Ԃ�ۂ��Ƃ��o����B

	�T���ɂ��Ă� class IArchive ���Q�Ƃ��邱�ƁB

�P�D�ۑ�����Ƃ�
�@CSerialize s;
�@IArchive�h���N���X v;
�@s << v; // ����Ŋi�[
�@vector<BYTE>& vData = *(s.GetData());
�@�����Ƃ́A�������A&vData[0]����AvData.size()�o�C�g����
�@�t�@�C���Ƃ��ĕۑ�����΁A�V���A���C�Y�����B

�Q�D�V���A���C�Y�����f�[�^��ǂݏo���Ƃ�
�@CSerialize s;
�@IArchive�h���N���X v;
�@vector<BYTE>& vData = *(s.GetData());
�@vData�ɂP�D�ŃV���A���C�Y�����t�@�C������ǂݍ��ށB
�@s.SetStoring(false); // ���܂���ǂݏo���˂�I
�@// �����̃t���O�����A���̂悤�ɃX�g���[������̓ǂݏo���ł����
�@// �錾��������΁A
�@s << v; // ����A�P�D�Ɠ����\�L�łn�j�B
�@����ŁA�X�g���[������̓ǂݏo���������B

�R�D��̎菇��IArchive�h���N���X���g��Ȃ��Ƃ��ۑ��j

�@CSerialize s;
�@int x,y,z;
�@s << x << y << z;
�@CSerialize s;
�@s.SetStoring(false); // ���̃t���O��|���Ă����Ύ��o��
�@int x,y,z;
�@s << x << y << z;

�@��P�D
�@int x = 1,y = 2,z = 3;
�@CSerialize s;
�@string str;
�@str = "����������";
�@s << str;
�@s << x << y << z; // �X�g���[���ɕۑ�

�@CFile file;
�@file.Save("test.dat",s.GetData()); // �ۑ����Ă݂悤�O�O�G
�@file.Load("test.dat",s.GetData()); // �ǂݍ���ł݂悤�O�O�G

// �� s.Save("test.dat");
// s.Load("test.dat"); �ł������Ӗ�

�@s.SetStoring(false); // �X�g���[������l��
�@string sy;
�@s << sy; // sy�ɂ́A�������������Ԃ��Ă���
�@s << z << y << x; // z,y,z�ɂ́A���ꂼ��1,2,3���Ԃ��Ă���

���ӓ_�F

CSerialize x,y,z;
�@x << y << z;��
�@x.SetStoring(false); // ���̃t���O��|���Ă����Ύ��o��
�@x << y << z;�ƕ����ł���B����́ACSerialize�̕ێ����Ă���
�@�A�[�J�C�u�́Avector<BYTE>�ł���Avector�̏����o�����ɂ́A
�@���̃T�C�Yvector::size() ��擪��int�f�[�^�Ƃ��ď����o���Ă��邽�߂ł���B����āA

�@CSerialize x,y;
�@int a,b;
�@y << a << b; // y��a,b���V���A���C�Y
�@x << y; // x��y���V���A���C�Y
�Ƃ����ꍇ�A
�@x.SetStoring(false); // ���̃t���O��|���Ă����Ύ��o��
�@x << a << b;
�ł͐��������o���Ȃ��B�Ȃ��Ȃ�΁A�擪��y�̃A�[�J�C�u�T�C�Y���������Ă��邩��ł���B�������菜�����߂ɁA�_�~�[��int�̒l�����o���Ă��Ɨǂ��B

�@int dummy;
�@x << dummy << a << b;

�z��ɑ΂��Ă��������V���A���C�Y�ł���
�@��
�@int n[5];
�@CSerialize x;
�@x.Store(&n[0],NELEMS(n));
���̕������A����Ƀ}�N���𗘗p���āA

x.Store(ArraySerialize(n));�Ə�����
�i�������A�ȉ��̃}�N���ɂ������Ȃ̂Ŕz��T�C�Y���킩���Ă���Ƃ��̂݁j

#define ArraySerialize(n) &n[0],NELEMS(n)

���Ƃ��΁Astring�ɑ΂��Ă������悤��
�@string s[5];
�@CSerialize x;
�@x.Store(ArraySerialize(s));�Ə�����B

�܂��AIArchive�h���N���X�ɑ΂��Ă����l��
�@CArchiveDerived ar[5];
�@CSerialize x;
�@x.Store(ArraySerialize(ar));�Ə����܂�

���@CSerialize�̃V���A���C�Y�Ɋւ��钍�ӎ����@��

CSerialize�́A���݂̃|�W�V�����E�i�[�����������ɃV���A���C�Y���܂��B

CSerialize s1,s2,s3;
s1 << s2;
�̂悤�ɂ����ꍇ�A���̂Ƃ�s2�́A���ʁACSerialize�̋L�^���́A�i�[�����ɂȂ��Ă���ł��傤���Apush_back���Ă����킯�ŁA���̊i�[�ʒu(GetPos�ŕԂ�)�́A�����ɂȂ��Ă��܂��B

����āAs1����A
s1.SetStoring(false);
s1 << s3;
�ƁA�f�V���A���C�Y�i�����j���Ă����As3���炷���Ƀf�[�^�����o�����Ƃ͏o���܂���Bs3����f�[�^���f�V���A���C�Y����ɂ́As3.SetStoring(false)�Ƃ��āA���o�������ɂ��ă|�W�V������擪�ɂ���Ƃ�����Ƃ��s�Ȃ��K�v������܂��B

�悭�n�}���̂ŁA���ӂ��܂��傤�B

*/
public:

	///	�f�[�^�̏������B
	///	�����̃X�g���[�����N���A���āAIsStoring()��true��Ԃ��悤�ɂȂ�
	virtual void	Clear();

	virtual bool	IsStoring() const { return m_bStoring; }
	///	�i�[�����𒲂ׂ�
	virtual bool*	GetStoring() { return &m_bStoring; }
	///	�i�[�����ւ̃|�C���^���擾����
	virtual void	SetStoring(bool b)
	{ m_bStoring = b; m_nDataPos = 0; if (b) GetData()->clear(); }
	/**	�������false��ݒ肵�āA�擾�i�����j�����ɂ����Ƃ��A
		�f�[�^�̂ǂ�����ǂݏo���Ă��邩������
		�f�[�^�|�W�V�����|�C���^���N���A����
		�f�[�^��ۑ������ɂ����Ƃ��́A�f�[�^���N���A����
		�i�������Ȃ��ƁA�O��̃f�[�^�ɏ㏑�����Ă��܂��j
	*/
	virtual void	InnerSetStoring(bool b) { m_bStoring = b; }
	///	�f�[�^�|�W�V�����̃��Z�b�g�E�f�[�^�N���A�����s�Ȃ킸�Ɋi�[��������
	///	�ύX����Ȃ�΁A���������g���ׂ�

	///	�X�g���[���̎擾
	virtual vector<BYTE>* GetData() { return& m_abyData; }

	//	------ �e��f�[�^�̊i�[�p�I�y���[�^
	///	�A�[�J�C�u�I
	virtual ISerialize& operator << (IArchive& vData);

	///	primitive data�ɑ΂���V���A���C�Y
	virtual ISerialize& operator << (int& nData);
	virtual ISerialize& operator << (bool& bData);
	virtual ISerialize& operator << (BYTE& byData);
	virtual ISerialize& operator << (string& strData);
	virtual ISerialize& operator << (WORD& wData){// �ǉ�
		return SerializeT(wData);
	}
	virtual ISerialize& operator << (DWORD& dwData){
		return SerializeT(dwData);
	}

	///	vector���I
	virtual ISerialize& operator << (vector<int>& anData);
	virtual ISerialize& operator << (vector<bool>& abData);
	virtual ISerialize& operator << (vector<BYTE>& abyData);
	virtual ISerialize& operator << (vector<string>& astrData);
	virtual ISerialize& operator << (vector<WORD>& awData);// �ǉ�
	virtual ISerialize& operator << (vector<DWORD>& adwData);// �ǉ�


	///	�������g���I�O�O�G
	virtual ISerialize& operator << (ISerialize& vData);


	//	-----------------------------------------------------------

	///	�����āA�V���A���C�Y�Ԃ̃R�s�[�I
	virtual ISerialize& operator = (ISerialize& vSeri);

	///	���܂�A���������̗p�ӂ������Ȃ����D�D
	virtual LRESULT Save(const string& filename);
	///	�X�g���[���̃t�@�C���ւ̕ۑ�
	virtual LRESULT Load(const string& filename);
	///	�X�g���[���̃t�@�C������̕���

	///	���݂̊i�[�ʒu�ւ̃|�C���^���擾
	///	�i�g���ق����������ǃf�o�b�O�p�ɁB�j
	virtual int*		GetPos() { return& m_nDataPos; }

	//	�R���X�g���N�^
	CSerialize() { Clear(); }
	virtual ~CSerialize() {}

protected:
	vector<BYTE> m_abyData;	//	�����Ƀf�[�^�͕ۑ������(������X�g���[���ƌĂ�)
	bool		m_bStoring; //�@�X�g���[���ɕۑ����Ȃ̂��A���o�����Ȃ̂��H
	int			m_nDataPos;	//�@�X�g���[������f�[�^�����o���Ƃ��A
							//	���݉��o�C�g�ڂ��w���Ă��邩������
};

///	�z��V���A���C�Y�̃w���p
#define ArraySerialize(n) &n[0],NELEMS(n)

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
