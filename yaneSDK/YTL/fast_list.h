/*
	fast_list	: O(log N)�Ō����ł���list
		programmed & desinged by yaneurao(M.Isozaki) '02/11/18
*/

#ifndef __YTLFastList_h__
#define __YTLFastList_h__

namespace yaneuraoGameSDK3rd {
namespace YTL {

template <class T>
class fast_list {
/**
	�L���b�V���V�X�e��
		�E�I�u�W�F�N�g�͔ԍ�(class T)�ŊǗ�
		�E�Q�Ƃ��邱�Ƃɂ���ă`�F�C���̐擪�ɂ���
		�E�`�F�C���̍Ō���i��ԎQ�Ƃ��Ă��Ȃ����́j��
			���o���@�\����
		�E�I�u�W�F�N�g�̐����^����ɂ͂��̃N���X�͊֗^���Ȃ�
*/
public:
	typedef list<T> listchain;
	typedef map<T, typename listchain::iterator> listchainmap;

	LRESULT ReferToObject(const T& t)
		/**
			ID�ɑ΂���I�u�W�F�N�g���Q�Ƃ������̂Ƃ��āA
			���̃I�u�W�F�N�g�ɑ΂���g�p�J�E���g�����Z����

			�Ԓl
			0: ���̃I�u�W�F�N�gID�͂��łɃ��X�g�ɑ��݂���
				�Q�Ƃ����Ƃ������ƂŁA���X�g�擪�Ɉړ�������
	
			1: ���̃I�u�W�F�N�gID�͑��݂��Ȃ�����
				�̂Ő擪�ɒǉ������B
		*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(t);
		if (it==GetMap()->end()){
			//	����key�͑��݂��Ȃ�
			GetList()->push_front(t);
			GetMap()->insert(pair<T,listchain::iterator>(t,GetList()->begin()));
//			m_funcCreateDelegate(n);
			return 1;
		}
		GetList()->erase(it->second);	//	���̗v�f��擪�Ɉړ�
		GetList()->push_front(t);		//	�擪�ɒǉ�
		it->second = GetList()->begin();//	map��iterator���������w���悤�ɕύX
		return 0;
	}


	LRESULT	releaseLast(T& t)
	/**
		�Ō�̂ЂƂ�(��Ԏg���Ă��Ȃ��I�u�W�F�N�g)���������

		�Ԓl
			0 : ������
				�i���̂Ƃ��At�ɉ������ID������j
			1 : ������ׂ��I�u�W�F�N�g������ 
	*/
	{
		listchain::reverse_iterator it = GetList()->rbegin();
		if (it==GetList()->rend()) {
			return 1; // ������ׂ��I�u�W�F������
		}
//		m_funcReleaseDelegate(*it);
		t = *it; // �������I
		GetMap()->erase(*it);   // �����map�ォ��Q�Ƃ��Ă���iterator���폜
		GetList()->erase(it.base());
		return 0;
	}

	LRESULT release(const T& id)
	/**
		����ID�Ŏw�肵���I�u�W�F�N�g���������

		�Ԓl
			0 : ������
			1 : �w�肳�ꂽ�I�u�W�F�N�g�͑��݂��Ȃ�����
	*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(id);
		if (it==GetMap()->end()){
			//	����key�͑��݂��Ȃ�
			return 1;
		}
		GetList()->erase(it->second);	//	���̗v�f���폜
		GetMap()->erase(it);
		return 0;
	}

	LRESULT	releaseAll()
		/**
			���ׂẴI�u�W�F�N�g(LookObject�ŎQ�Ƃ�������)��
			�������B

			����́AsetReleaseDelegate�Őݒ肵���֐���
			�Ăяo�����Ƃɂ���čs�Ȃ��Ȃ�ȉ��̂悤�ɏ����ׂ����D�D
		listchain::iterator it = GetList()->begin();
		while (it!=GetList()->end()){
			m_funcReleaseDelegate(*it);
			it++;
		}
		GetList()->clear();
		GetMap()->clear();
		*/
	{
		GetList()->clear(); GetMap()->clear();
		//�@�I�u�W�F�N�g�̐����^����ɂ͂��̃N���X�͊֗^���Ȃ�

		return 0;
	}

	bool	isExist(const T& id)
	///	����ID�ɑΉ�����I�u�W�F�N�g�����݂��邩���ׂ�
	///	���݂���Ȃ��true���Ԃ�

	/**
		�ȉ���GetList��GetMap�́A�݂��ɘA�����Ă���̂�
		�����ۂ������̍폜���͕s��
	*/
	{
		listchainmap::iterator it;
		it = GetMap()->find(id);
		return it!=GetMap()->end();
	}

	listchain* GetList() { return& m_list; }
	///	�Q�Ƃ����ԍ����̃��X�g�B���̌���ɂ�����̂��Â����̂�����
	///	�����炩��j�����郈���V

	listchainmap* GetMap() { return &m_map; }
	///	list���Ɛ��`�T���ɂȂ��đ�K�͂��ƒx���̂ŁAiterator��ʓr�ێ�

protected:
	listchain m_list;
	//	�Q�Ƃ����ԍ����̃��X�g�B���̌���ɂ�����̂��Â����̂�����
	//	�����炩��j�����郈���V

	listchainmap m_map;
	//	list���Ɛ��`�T���ɂȂ��đ�K�͂��ƒx���̂ŁAiterator��ʓr�ێ�
};

} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd

#endif
