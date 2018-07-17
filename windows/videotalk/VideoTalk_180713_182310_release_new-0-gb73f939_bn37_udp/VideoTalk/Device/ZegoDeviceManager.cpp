#include "ZegoDeviceManager.h"
#include "Signal/ZegoSDKSignal.h"
#include "ZegoVideoTalkDemo.h"
#include "Base/IncludeZegoLiveRoomApi.h"

ZegoDeviceManager::ZegoDeviceManager()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: device manager module create").arg(__FUNCTION__)));
	connect(GetAVSignal(), &QZegoAVSignal::sigAudioDeviceChanged, this, &ZegoDeviceManager::OnAudioDeviceStateChanged);
	connect(GetAVSignal(), &QZegoAVSignal::sigVideoDeviceChanged, this, &ZegoDeviceManager::OnVideoDeviceStateChanged);

	//��ʼ��ʱ��˷硢����ͷ��������Ĭ�Ͽ���
	LIVEROOM::EnableMic(m_micEnabled);
	LIVEROOM::EnableCamera(m_CameraEnabled);
	LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
}

ZegoDeviceManager::~ZegoDeviceManager()
{
	log_string_notice(qtoc(QStringLiteral("[%1]: device manager module destroy").arg(__FUNCTION__)));
}

//�豸��ʼ��
void ZegoDeviceManager::EnumAudioDeviceList()
{
	int nDeviceCount = 0;
	AV::DeviceInfo* pDeviceList(NULL);

	m_audioDeviceList.clear();
	//��ȡ��Ƶ�����豸
	pDeviceList = LIVEROOM::GetAudioDeviceList(AV::AudioDeviceType::AudioDevice_Input, nDeviceCount);
	log_string_notice(qtoc(QStringLiteral("[%1]: get audio input device list, device count: %2")
		.arg(__FUNCTION__)
		.arg(nDeviceCount)
	));

	for (int i = 0; i < nDeviceCount; i++)
	{
		QDeviceInfo info;
		info.deviceId = pDeviceList[i].szDeviceId;
		info.deviceName = pDeviceList[i].szDeviceName;

		m_audioDeviceList.append(info);
	}

	if (m_audioDeviceList.size() > 0)
	{
		m_micListIndex = 0;
		m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
		
		emit sigMicIdChanged(m_audioDeviceId);
	}
	else
	{
		m_audioDeviceId = "";
	}

	LIVEROOM::FreeDeviceList(pDeviceList);
	pDeviceList = NULL;

}

//�豸��ʼ��
void ZegoDeviceManager::EnumVideoDeviceList()
{
	int nDeviceCount = 0;
	AV::DeviceInfo* pDeviceList(NULL);

	m_videoDeviceList.clear();
	//��ȡ��Ƶ�豸
	pDeviceList = LIVEROOM::GetVideoDeviceList(nDeviceCount);
	for (int i = 0; i < nDeviceCount; i++)
	{
		QDeviceInfo info;
		info.deviceId = pDeviceList[i].szDeviceId;
		info.deviceName = pDeviceList[i].szDeviceName;

		m_videoDeviceList.append(info);
	}
	//��������ͷ1
	if (m_videoDeviceList.size() > 0)
	{
		m_cameraListIndex = 0;
		m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
		//emit sigCameraIdChanged(m_videoDeviceId);
	}
	else
	{
		m_videoDeviceId = "";
	}


	LIVEROOM::FreeDeviceList(pDeviceList);
	pDeviceList = NULL;
}

void ZegoDeviceManager::RefreshMicIndex()
{
	m_micListIndex = -1;
	for (int i = 0; i < m_audioDeviceList.size(); i++)
		if (m_audioDeviceList.at(i).deviceId == m_audioDeviceId)
			m_micListIndex = i;
}

void ZegoDeviceManager::RefreshCameraIndex()
{
	m_cameraListIndex = -1;
	for (int i = 0; i < m_videoDeviceList.size(); i++)
		if (m_videoDeviceList.at(i).deviceId == m_videoDeviceId)
			m_cameraListIndex = i;
}

//��Ƶ�豸�л�
QDeviceState ZegoDeviceManager::SetMicrophoneIdByIndex(int index)
{
	if (index >= m_audioDeviceList.size())
		return STATE_ERROR;

	m_micListIndex = index;
	m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
	emit sigMicIdChanged(m_audioDeviceId);

	log_string_notice(qtoc(QStringLiteral("[%1]: set microphone deviceId: %2 by index %3")
		.arg(__FUNCTION__)
		.arg(m_audioDeviceId)
		.arg(index)
	));

	return STATE_NORMAL;
}

QDeviceState ZegoDeviceManager::SetCameraIdByIndex(int index)
{
	if (index >= m_videoDeviceList.size())
		return STATE_ERROR;

	m_cameraListIndex = index;
	m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
	emit sigCameraIdChanged(m_videoDeviceId);

	log_string_notice(qtoc(QStringLiteral("[%1]: set speaker deviceId: %2 by index %3")
		.arg(__FUNCTION__)
		.arg(m_videoDeviceId)
		.arg(index)
	));

	return STATE_NORMAL;
}

int ZegoDeviceManager::GetAudioDeviceIndex()
{
	return m_micListIndex;
}

int ZegoDeviceManager::GetVideoDeviceIndex()
{
	return m_cameraListIndex;
}

//�����л�
void ZegoDeviceManager::SetMicVolume(int volume)
{
	m_micVolume = volume;

    //log_string_notice(tr("SetMicVolume: %1").arg(volume).toStdString().c_str());
	LIVEROOM ::SetMicDeviceVolume(qtoc(m_audioDeviceId), volume);
    if (volume == 0 && m_micEnabled)
    {
        m_micEnabled = false;

		LIVEROOM::EnableMic(m_micEnabled);
		//emit sigMicVolumeMute(!m_micEnabled);
    }
    else if (!m_micEnabled && volume > 0)
    {
        m_micEnabled = true;

		LIVEROOM::EnableMic(m_micEnabled);
		//emit sigMicVolumeMute(!m_micEnabled);
    }
    
	
}

int ZegoDeviceManager::GetMicVolume()
{
	return m_micVolume;
}

void ZegoDeviceManager::SetMicEnabled(bool isUse)
{
	m_micEnabled = isUse;
	LIVEROOM::EnableMic(isUse);
}

bool ZegoDeviceManager::GetMicEnabled()
{
	return m_micEnabled;
}

//���������л�
void ZegoDeviceManager::SetPlayVolume(int volume)
{
	m_playVolume = volume;

	LIVEROOM::SetPlayVolume(m_playVolume);
	if (volume == 0 && m_SpeakerEnabled)
	{
		m_SpeakerEnabled = false;

		LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
		//emit sigSpeakerVolumeMute(!m_SpeakerEnabled);
	}
	else if (volume > 0 && !m_SpeakerEnabled)
	{
		m_SpeakerEnabled = true;

		LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
		//emit sigSpeakerVolumeMute(!m_SpeakerEnabled);
	}
}

int ZegoDeviceManager::GetPlayVolume()
{
	return m_playVolume;
}

void ZegoDeviceManager::SetSpeakerEnabled(bool isUse)
{
	m_SpeakerEnabled = isUse;
	LIVEROOM::EnableSpeaker(m_SpeakerEnabled);
}

bool ZegoDeviceManager::GetSpeakerEnabled()
{
	return m_SpeakerEnabled;
}

void ZegoDeviceManager::SetCameraEnabled(bool isUse)
{
	m_CameraEnabled = isUse;
	LIVEROOM::EnableCamera(m_CameraEnabled);
}

bool ZegoDeviceManager::GetCameraEnabled()
{
	return m_CameraEnabled;
}

QVector<QDeviceInfo> ZegoDeviceManager::GetAudioDeviceList()
{
	return m_audioDeviceList;
}

QVector<QDeviceInfo> ZegoDeviceManager::GetVideoDeviceList()
{
	return m_videoDeviceList;
}

const QString& ZegoDeviceManager::GetAudioDeviceId()
{
	return m_audioDeviceId;
}

const QString& ZegoDeviceManager::GetVideoDeviceId()
{
	return m_videoDeviceId;
}

void ZegoDeviceManager::OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state)
{
	if (deviceType == AV::AudioDeviceType::AudioDevice_Output)
		return;

	if (state == AV::DeviceState::Device_Added)
	{
		QDeviceInfo added_device;
		added_device.deviceId = strDeviceId;
		added_device.deviceName = strDeviceName;
		m_audioDeviceList.append(added_device);

		//��0��1
		if (m_audioDeviceList.size() == 1)
		{
			m_micListIndex = 0;
			m_audioDeviceId = m_audioDeviceList.at(m_micListIndex).deviceId;
			emit sigMicIdChanged(m_audioDeviceId);

			//emit sigDeviceExist(TYPE_AUDIO);
		}

		emit sigDeviceAdded(TYPE_AUDIO, added_device.deviceName);
	}
	else if (state == AV::DeviceState::Device_Deleted)
	{
		
		for (int i = 0; i < m_audioDeviceList.size(); ++i)
		{
			if (m_audioDeviceList.at(i).deviceId != strDeviceId)
				continue;

			m_audioDeviceList.takeAt(i);

			if (m_micListIndex == i)
			{
				if (m_audioDeviceList.size() > 0)
				{
					m_audioDeviceId = m_audioDeviceList.at(0).deviceId;
					
				}
				else
				{
					m_audioDeviceId = "";
					emit sigDeviceNone(TYPE_AUDIO);
				}

				RefreshMicIndex();
				emit sigMicIdChanged(m_audioDeviceId);
			}

			emit sigDeviceDeleted(TYPE_AUDIO, i);
		}

	}

}

void ZegoDeviceManager::OnVideoDeviceStateChanged(const QString& strDeviceId, const QString& strDeviceName, AV::DeviceState state)
{
	if (state == AV::DeviceState::Device_Added)
	{
		QDeviceInfo added_device;
		added_device.deviceId = strDeviceId;
		added_device.deviceName = strDeviceName;
		m_videoDeviceList.append(added_device);

		if (m_videoDeviceList.size() == 1)
		{
			m_cameraListIndex = 0;
			m_videoDeviceId = m_videoDeviceList.at(m_cameraListIndex).deviceId;
			emit sigCameraIdChanged(m_videoDeviceId);
			emit sigDeviceExist(TYPE_VIDEO);
		}

		emit sigDeviceAdded(TYPE_VIDEO, added_device.deviceName);
	}
	else if (state == AV::DeviceState::Device_Deleted)
	{
		for (int i = 0; i < m_videoDeviceList.size(); ++i)
		{
			if (m_videoDeviceList.at(i).deviceId != strDeviceId)
				continue;

			m_videoDeviceList.takeAt(i);

			if (m_cameraListIndex == i)
			{
				//����ǰ��������ͷ���ڣ���δ������ͷ2ʹ�ã���δ��ʹ�õ�����ͷ������1��
				if (m_videoDeviceList.size() > 0)
				{
					m_videoDeviceId = m_videoDeviceList.at(0).deviceId;
				}
				else
				{
					m_videoDeviceId = "";
					emit sigDeviceNone(TYPE_VIDEO);
				}

				//ˢ��index
				RefreshCameraIndex();
				emit sigCameraIdChanged(m_videoDeviceId);
			}

			emit sigDeviceDeleted(TYPE_VIDEO, i);
		}
	}
}
