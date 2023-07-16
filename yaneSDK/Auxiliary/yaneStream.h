
//	�e�L�X�g���o�͗p�X�g���[��
//		(CDebugWindow�����A��������h��)
//	�o�C�i�����o�͗p�X�g���[��
//		(CFile�����A��������h��)

#ifndef __yaneStream_h__
#define __yaneStream_h__


//////////////////////////////////////////////////////////////////////////

namespace yaneuraoGameSDK3rd {
namespace Auxiliary {

class IOutputStream {
/**
	�o�̓X�g���[��
	//	���肩���D�D
*/
public:
	virtual ~IOutputStream(){}
};

class ITextOutputStream {
/**
	�e�L�X�g�o�͗p�̃X�g���[��

	�����FOut�́A��x��512����(�I��������\0���܂�)�܂ł���
			�o�͂��T�|�[�g���Ȃ��B
*/
public:
	virtual void	Clear() {}						///	�o�͐���N���A����
	virtual void __cdecl Out(LPCSTR fmt, ... ) {}	///	�������\������
	virtual void	Out(int);						///	������\������
	virtual void	Out(const string&);				///	��������o�͂���

	virtual ~ITextOutputStream(){}	///	interface�Ȃ̂ŉ��z�f�X�g���N�^���K�v
};

class CTextOutputStreamNullDevice : public ITextOutputStream {
/**
	class ITextOutputStream ��Null Device(�������Ȃ��f�o�C�X)
*/
public:
	virtual void	Clear() {}
	virtual void __cdecl Out(LPCSTR fmt, ... ) {}
	virtual void	Out(int){}
	virtual void	Out(const string&){}
};

//	��̂�̃t�@�C���o�̓^�C�v
class CTextOutputStreamFile : public ITextOutputStream {
/**
	class ITextOutputStream �̋�ۃN���X
	�i�t�@�C���ɒ���o�͂���^�C�v�j

	Out�̂��ƂɁA�t�@�C���ɏo�͂��s�Ȃ�
*/
public:
	virtual void	Clear();
	///	�t�@�C�����폜

	virtual void __cdecl Out(LPCSTR fmt, ... );
	///	�������\������

	virtual void	SetFileName(const string& str) { m_strFileName = str; Clear(); }
	///	�t�@�C������ݒ肷��
	///	(���̂Ƃ��A���̃t�@�C�����̃t�@�C�������݂���̂Ȃ�΃N���A����)

	virtual string	GetFileName() const { return m_strFileName; }
	///	�t�@�C�������擾����
	///	�ȍ~�A�����Őݒ肵���t�@�C�����̃t�@�C���ɑ΂��ďo�͂���悤�ɂȂ�

protected:
	string m_strFileName;
};

//	��̂��Visual Studio�̃f�o�b�O�E�B���h�D�ɏo�͂���^�C�v
class CTextOutputStreamVSDebug : public ITextOutputStream {
/**
	class ITextOutputStream �̋�ۃN���X
	�iVisual Studio�̃f�o�b�O�E�B���h�D�ɏo�͂���^�C�v�j
*/
public:
	virtual void __cdecl Out(LPCSTR fmt, ... ) {
		CHAR	buf[512];
		::wvsprintf(buf, fmt, (va_list)(&fmt+1));
		OutputDebugString(buf);
	}	///	�������\������
};

class CTextOutputStreamVersatile : public ITextOutputStream {
/**
		���ׂẴf�o�C�X�̏o�͂ł���f�o�C�X

		�G���[�o�͗p�f�o�C�XErr�́A
		����class CTextOutputStreamVersatile �̃C���X�^���X
*/
public:
	virtual void	SelectDevice(const smart_ptr<ITextOutputStream>& device)
	{ m_vDevice = device; }
	/**
		�o�͂���f�o�C�X�̃X�}�[�g�|�C���^��n��
		�ȍ~�A�����ɏo�͂���悤�ɂȂ�
	*/

	void	Debug(){
		/**
			�f�o�b�O�p�ɁAError.txt�Ƃ����t�@�C���ɏo�͂���
		*/
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		SelectDevice(smart_ptr<ITextOutputStream>(p));
	}

	virtual void	Clear() { m_vDevice->Clear();	}
	virtual void __cdecl Out(LPCSTR fmt, ... );	// {	m_vDevice->Out(fmt);}
	//	���ψ����̈Ϗ��͕s���H
	virtual void	Out(int n){	m_vDevice->Out(n);	}
	virtual void	Out(const string&s){ m_vDevice->Out(s); }

	CTextOutputStreamVersatile()
#ifndef COMPILE_YANE_PLUGIN_DLL	//	plug-in DLL���쐬���Ȃ��ꍇ�̂�
		: m_vDevice(new CTextOutputStreamNullDevice)
	//	static�I�u�W�F��new�����Ⴂ����R(`�D�L)�m>DLL
#endif
	{}
		//	�ŏ��ANullDevice���������Ă���

protected:
	smart_ptr<ITextOutputStream> m_vDevice;
};

#ifdef USE_ErrorLog		//	CErrorLog�N���X��L���ɂ���B
	//	�G���[�o�̓f�o�C�X���ЂƂp��
	extern CTextOutputStreamVersatile Err;
#else
	extern CTextOutputNullDevice Err;
	//	�����Ȃ����Ƃ��΍œK���ŏ�����ł��傤�D�D
#endif
/**
	�G���[���O���t�@�C���ɏo�͂����F
	{
		CTextOutputStreamFile* p = new CTextOutputStreamFile;
		p->SetFileName("Error.txt");
		Err.SelectDevice(smart_ptr<ITextOutputStream>(p));
	}
*/

//////////////////////////////////////////////////////////////////////////

class IInputStream {
/**
	���͗p�X�g���[�����N���X
	class IOutputStream�����Q�l�̂��ƁB
	//	���肩��
*/
public:


	virtual ~IInputStream(){}
};

//////////////////////////////////////////////////////////////////////////

} // end of namespace Auxiliary
} // end of namespace yaneuraoGameSDK3rd

#endif
