#include "ZegoMainDialog.h"
#include "Base/ZegoVideoTalkDefines.h"
#include "Base/ZegoVideoTalkVersion.h"
#include <QDebug>

ZegoMainDialog::ZegoMainDialog(QWidget *parent)
	: ZegoDialog(parent)
{
	ui.setupUi(this);

	ui.m_edRoomID->installEventFilter(this);

	//不输入房间号不能进入房间
	ui.m_bEnterRoom->setEnabled(false);
	connect(ui.m_edRoomID, &QLineEdit::textChanged, this, [this] { emit sigCheckEnterRoom(); });

	connect(this, &ZegoMainDialog::sigCheckEnterRoom, this, &ZegoMainDialog::OnCheckEnterRoom);

	ui.m_lbVersion->setText(QStringLiteral("v %1").arg(APP_VERSION));
}

ZegoMainDialog::~ZegoMainDialog()
{

}

void ZegoMainDialog::initDialog()
{
	initButtonIcon();

	//读取用户配置，若不存在则新建配置
	mConfig.LoadConfig();

	m_strEdUserId = mConfig.GetUserId();
    m_strEdUserName = mConfig.getUserName();

	bool isUseTestEnv = mConfig.GetUseTestEnv();
	mBase.setTestEnv(isUseTestEnv);

	updateAppVersionTitle();

	SettingsPtr pCurSettings = mConfig.GetVideoSettings();
	if (pCurSettings != nullptr)
	{
		mBase.InitAVSDK(pCurSettings, m_strEdUserId, m_strEdUserName);
	}
}

void ZegoMainDialog::initButtonIcon()
{
	ui.m_bClose->setButtonIcon("close");
	ui.m_bMin->setButtonIcon("min");

	ui.m_bJumpToNet->setButtonIcon("official");
	ui.m_bJumpToNet->setToolTip(tr("关于我们"));
}

void ZegoMainDialog::updateAppVersionTitle()
{
	AppVersion appVersion = mConfig.getAppVersion();
	switch (appVersion.m_versionMode)
	{
	case ZEGO_PROTOCOL_UDP:
		ui.m_title->setText(tr("VideoTalk (国内版)"));
		break;
	case ZEGO_PROTOCOL_UDP_INTERNATIONAL:
		ui.m_title->setText(tr("VideoTalk (国际版)"));
		break;
	case ZEGO_PROTOCOL_CUSTOM:
		ui.m_title->setText(tr("VideoTalk (自定义)"));
		break;
	default:
		ui.m_title->setText(tr("VideoTalk (未知)"));
		break;
	}
}

//UI信号槽
void ZegoMainDialog::on_m_bEnterRoom_clicked()
{
	QString roomID = ui.m_edRoomID->text();
	if (roomID.isEmpty())
	{
		QMessageBox::warning(NULL, tr("警告"), tr("房间号不能为空"));
		return;
	}

	QString strUserId = mConfig.GetUserId();
	QString strUserName = mConfig.getUserName();

	if (!LIVEROOM::SetUser(strUserId.toStdString().c_str(), strUserName.toStdString().c_str()))
	{
		QMessageBox::warning(NULL, tr("警告"), tr("用户ID或用户名错误"));
		return;
	}

	RoomPtr pRoom(new QZegoRoomModel(roomID, QString(""), strUserId, strUserName));
	ZegoVideoTalkDialog videotalk(pRoom, this, m_isVideoCustom);
	videotalk.initDialog();
	this->hide();
	videotalk.exec();
}

void ZegoMainDialog::on_m_bSettings_clicked()
{
	ZegoSettingsDialog settings;
	//connect(&settings, &ZegoSettingsDialog::sigReturnConfigToMainDialog, this, &ZegoMainDialog::OnReturnConfigFromSettingsDialog);
	settings.initDialog();
	settings.exec();

	updateAppVersionTitle();
}

/*void ZegoMainDialog::OnReturnConfigFromSettingsDialog(QZegoUserConfig userConfig)
{
	m_userConfig.SetUserId(userConfig.GetUserId());
	m_userConfig.SetUserName(userConfig.getUserName());
	m_userConfig.SetVideoSettings(userConfig.GetVideoSettings());
	m_userConfig.SetVideoQuality(userConfig.GetVideoQuality());
	m_userConfig.setAppVersion(userConfig.getAppVersion());
	
	m_strEdUserId = m_userConfig.GetUserId();
	m_strEdUserName = m_userConfig.getUserName();
}*/

void ZegoMainDialog::on_m_bClose_clicked()
{
	this->close();
}

void ZegoMainDialog::on_m_bMin_clicked()
{
	this->showMinimized();
}

void ZegoMainDialog::on_m_bJumpToNet_clicked()
{
	QDesktopServices::openUrl(QUrl(QLatin1String("https://www.zego.im")));
}

void ZegoMainDialog::OnCheckEnterRoom()
{
	QString roomId = ui.m_edRoomID->text();
	roomId = roomId.simplified();

	if (!roomId.isEmpty())
		ui.m_bEnterRoom->setEnabled(true);
	else
		ui.m_bEnterRoom->setEnabled(false);
}

bool ZegoMainDialog::eventFilter(QObject *target, QEvent *event)
{
	if (target == ui.m_edRoomID) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
			{
				on_m_bEnterRoom_clicked();
				return true;
			}
		}
	}

	return QDialog::eventFilter(target, event);
}