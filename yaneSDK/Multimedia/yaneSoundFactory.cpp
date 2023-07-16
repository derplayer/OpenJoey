#include "stdafx.h"

//	Sound�̃C���^�[�t�F�[�X
#include "yaneSound.h"

//	DirectSound, DirectMusic
#include "yaneDirectMusic.h"
#include "yaneDirectSound.h"
#include "yaneWaveOutput.h"
#include "yaneSoundParameter.h"

//	Sound�̎���
#include "yaneMIDIOutputMCI.h"
#include "yaneMIDIOutputDM.h"
#include "yaneWaveSound.h"

#include "yaneSoundFactory.h"

namespace yaneuraoGameSDK3rd {
namespace Multimedia {

CSoundFactory::CSoundFactory()
{
	m_nMIDIFactoryFirst	 = 2;
	m_nMIDIFactorySecond = 1;

	// default parameter
	m_pSoundParameter.Add();

	// �f�t�H���g�ŃX�g���[�����Ȃ�
	m_bStreamPlay = false;

	//	������factory�Ƃ��Đݒ肵�Ă����̂�
	CSound::SetFactory(smart_ptr<ISoundFactory>(this,false));
}

CSoundFactory::~CSoundFactory()
{
}

smart_ptr<ISound> CSoundFactory::Create(const string& filename)
{
	string ext = CFile::GetSuffixOf(filename);
	CFile::ToLower(ext);

	// �������Ȏ���(��)
	smart_ptr<ISound> p;
	if (ext=="mid") {
		p = CreateMIDI();
	} else {
		p = CreateSound();
	}
	if (p->Open(filename)!=0)
		{ p.Add(new CSoundNullDevice); }
	//	��������Open�����̂ł́AOpen�̐��ۂ��킩��Ȃ��̂�
	//	Loader��Open���ׂ�??

	return p;
}
	
smart_ptr<ISound> CSoundFactory::CreateSound(){
	return InnerCreate(3);
}

smart_ptr<ISound>	CSoundFactory::Create(const string& filename,bool bStreamPlay){
	return InnerCreate(4 + (bStreamPlay?1:0));
}

smart_ptr<ISound> CSoundFactory::CreateMIDI(){
	smart_ptr<ISound> p(InnerCreate(m_nMIDIFactoryFirst));
	if (p->GetType()==0 /*isNull()*/ ){
		p = InnerCreate(m_nMIDIFactorySecond);
	}
	return p;
}

smart_ptr<ISound> CSoundFactory::InnerCreate(int nDevice){
	ISound* pSound = NULL;
	smart_ptr<ISound> p;

	switch (nDevice){
	case 0 : pSound = new CSoundNullDevice; break;
	case 1 : if (CMIDIOutputMCI::CanUseMCIMIDI()) {
				pSound = new CMIDIOutputMCI();
			}
			break;
	case 2 : if (CDirectMusic::CanUseDirectMusic()) {
				// DirectMusic�g���Ł[
				IncRefDirectMusic();
				pSound = new CMIDIOutputDM(GetDirectMusic());

				// DirectMusic�̎Q�ƃJ�E���g�̃f�N�������g�̂��߂�
				// ���ʂȉ�̃I�u�W�F�N�g��s�܂��āAsmart_ptr�𐶐�����
				smart_ptr<function_callback>
					fn(function_callback_v::Create(&CSoundFactory::InnerDeleteChainForDM, this, pSound));
				p.Add(pSound, new nonarray_callback_ref_object<ISound>(pSound, fn));
				//	chain�ɒǉ�����
				GetSoundList()->insert(pSound);
				return p;
			}
			break;
	case 3 :
	case 4 :
	case 5 :
		{
			if (CDirectSound::CanUseDirectSound()) {
				// WaveOutput�g���Ł[
				IncRefWaveOutput();
				GetWaveOutput()->SetSoundParameter(GetSoundParameter());
				pSound = new CWaveSound(GetWaveOutput());

				bool bStreamPlay;
				if (nDevice==3) bStreamPlay=IsStreamPlay();
				else if (nDevice==4) bStreamPlay = false;
				else if (nDevice==5) bStreamPlay = true;

				// �D�L���������Őݒ�
				((CWaveSound*)pSound)->SetStreamPlay(bStreamPlay);

				// WaveOutput�̎Q�ƃJ�E���g�̃f�N�������g�̂��߂�
				// ���ʂȉ�̃I�u�W�F�N�g��s�܂��āAsmart_ptr�𐶐�����
				smart_ptr<function_callback>
					fn(function_callback_v::Create(&CSoundFactory::InnerDeleteChainForWO, this, pSound));
				p.Add(pSound, new nonarray_callback_ref_object<ISound>(pSound, fn));
				//	chain�ɒǉ�����
				GetSoundList()->insert(pSound);
				return p;
			}
		}
			break;
//	case 6 : break;	//	�ȉ���
	}

	if (pSound!=NULL){
		// ��̃I�u�W�F�N�g��s�܂��āAsmart_ptr�𐶐�����
		smart_ptr<function_callback>
			fn(function_callback_r<bool>::Create(&CSoundFactory::InnerDeleteChain,this,pSound));
		p.Add(pSound,new nonarray_callback_ref_object<ISound>(pSound,fn));
		// chain�ɒǉ�����
		GetSoundList()->insert(pSound);
	} else {
		p.Add(new CSoundNullDevice);
	}
	return p;
}

bool CSoundFactory::InnerDeleteChain(ISound* p){
	return GetSoundList()->erase(p);
}

void CSoundFactory::InnerDeleteChainForWO(ISound* p){
	// DirectSound�g���I������Ł[
	if (InnerDeleteChain(p)) {
		DecRefWaveOutput();
	}
}

void CSoundFactory::InnerDeleteChainForDM(ISound* p){
	// DirectMusic�g���I������Ł[
	if (InnerDeleteChain(p)) {
		DecRefDirectMusic();
	}
}

void CSoundFactory::IncRefDirectSound()
{
	m_vDirectSound.inc_ref();
}
CDirectSound* CSoundFactory::GetDirectSound()
{
	return m_vDirectSound.get();
}
void CSoundFactory::DecRefDirectSound()
{
	m_vDirectSound.dec_ref();
}

void CSoundFactory::IncRefDirectMusic()
{
	m_vDirectSound.inc_ref();
	m_vDirectMusic.SetOutClass(GetDirectSound());
	m_vDirectMusic.inc_ref();
}
CDirectMusic* CSoundFactory::GetDirectMusic()
{
	return m_vDirectMusic.get();
}
void CSoundFactory::DecRefDirectMusic()
{
	m_vDirectMusic.dec_ref();
	m_vDirectSound.dec_ref();
}

void CSoundFactory::IncRefWaveOutput()
{
	m_vDirectSound.inc_ref();
	m_vWaveOutputDS.SetOutClass(GetDirectSound());
	m_vWaveOutputDS.inc_ref();
}
IWaveOutput* CSoundFactory::GetWaveOutput()
{
	return m_vWaveOutputDS.get();
}
void CSoundFactory::DecRefWaveOutput()
{
	m_vWaveOutputDS.dec_ref();
	m_vDirectSound.dec_ref();
}

} // end of namespace Multimedia
} // end of namespace yaneuraoGameSDK3rd
