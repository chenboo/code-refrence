android端的shareboard的打开和关闭与windows端的处理不一样，主要是
因为android端不存在类似shareboard的dll，而是嵌入到主界面的framelayout，
首先讲一下打开的流程，

一、初始化网络
1》在windows教师端点击shareboard按钮后会向学生端发送CMD_SHAREBOARD | CMD_START_FUNCTION | CMD_MODIFY_STATUS命令
2》android学生端的StudentMainWork.cpp的函数
void CStudentMainWork::ProcessMemberAction(PACTION_ITEM pItem)中收到这个命令
并做处理：
1》case CMD_SHAREBOARD:
	if (pCommandPacket->dwCommand & CMD_STOP_FUNCTION)
	{
		if (m_memberInfo.dwState & CMD_SHAREBOARD)
		{
			m_memberInfo.dwState &= ~CMD_SHAREBOARD;
			StopShareBoardPassive();
		}
	}
	else
	{
		if (!(m_memberInfo.dwState & CMD_SHAREBOARD))
		{
			PSHAREBOARDPARAMS pShareBoardParams =
					(PSHAREBOARDPARAMS) pCommandPacket->abParams;
			StartShareBoardPassive(pShareBoardParams);//进入点
			m_memberInfo.dwState |= CMD_SHAREBOARD;
		}
	}
	break;

2》在函数StartShareBoardPassive中
BOOL CStudentMainWork::StartShareBoardPassive(
		PSHAREBOARDPARAMS pShareBoardParams)
{
	StopAll();
	FileSubmit_Stop();

	LIB_SHAREBOARD_PARAMS params = { 0 };
	params.pJVM = m_pJVM;
	params.objectShareBoardJNIBinder = m_jobj_MN_ShareBoardJNIBinder;

	params.dwLocalInterface = m_memberInfo.dwIpAddress;
	params.dwTeacherIp = m_dwTeacherIp;

	memcpy(params.szName, m_memberInfo.szDefaultName, 32 * sizeof(UTF16));
	memcpy(&(params.params), pShareBoardParams, sizeof(SHAREBOARDPARAMS));

	return ShareBoard_CreateClient(params);//这是一个全局函数，进入点
}

3》在ShareBoard_CreateClient函数中
BOOL ShareBoard_CreateClient(const LIB_SHAREBOARD_PARAMS& params) {
	LogDebug("jni", "ShareBoard_CreateClient");

	g_pShareBoarcMainWork = new ShareBoardMainWork();

	if (!g_pShareBoarcMainWork)
		return FALSE;

	if (!g_pShareBoarcMainWork->InitJNI(params)) {//进入点
		delete g_pShareBoarcMainWork;
		g_pShareBoarcMainWork = NULL;
		return FALSE;
	}

	return TRUE;
}

4》在ShareBoardMainWork::InitJNI函数中
BOOL ShareBoardMainWork::InitJNI(const LIB_SHAREBOARD_PARAMS& params) {
	memcpy(&m_params, &params, sizeof(LIB_SHAREBOARD_PARAMS));
	memcpy(m_szName, params.szName, sizeof(UTF16) * 32);
	m_dwLocalInterface = params.dwLocalInterface;

	m_pJVM = params.pJVM;
	m_jobj_ShareBoardJNIBinder = params.objectShareBoardJNIBinder;

	if (m_pJVM == NULL || m_jobj_ShareBoardJNIBinder == NULL)
		return FALSE;

	bool attached = false;
	JNIEnv* env = JniHelper::TryAttachCurrentThread(m_pJVM, attached);
	if (env == NULL) {
		return FALSE;
	}

	jclass clazzShareBoardJNIBinder = env->GetObjectClass(
			m_jobj_ShareBoardJNIBinder);
	if (clazzShareBoardJNIBinder) {
		m_jmethodID_JNICTJNativeNotify = env->GetMethodID(
				clazzShareBoardJNIBinder, "JNICTJNativeNotify", "(I[B)V");

		m_jmethodID_JNICTJPostMessage = env->GetMethodID(
				clazzShareBoardJNIBinder, "JNICTJPostMessage", "(III)V");

		m_jmethodID_JNICTJCreateTimer = env->GetMethodID(
				clazzShareBoardJNIBinder, "JNICTJCreateTimer", "(II)Z");
		m_jmethodID_JNICTJReleaseTimer = env->GetMethodID(
				clazzShareBoardJNIBinder, "JNICTJReleaseTimer", "(I)V");
	}

	// detach if necessary
	JniHelper::TryDetachCurrentThread(m_pJVM, attached);

	//NetWork
	if (!InitNetWork()) {
		UninitJNI();
		return FALSE;
	}

	JNICTJNativeNotify(NEV_FUNCTION_SET_SHAREBOARD_PARAMS, sizeof(DWORD),
			(BYTE*) &(m_params.dwLocalInterface));

	m_pThreadSendPacket = new pthread_t;
	if (NULL == m_pThreadSendPacket) {
		return FALSE;
	}

	pipe(m_PipeSendData);
	pipe(m_PipeEndThread);

	pthread_create(m_pThreadSendPacket, NULL, SendDataThread, this);

	JNICTJCreateTimer(ID_TIMER_NEEDFEEDBACK, 3000);
	JNICTJCreateTimer(ID_TIMER_RESTORE_SHARE_STATE, 1000);

	return TRUE;
}

5》函数ShareBoardMainWork::InitNetWork()中
BOOL ShareBoardMainWork::InitNetWork() {
	do {

		m_RecvBuffer.Initialize(400);
		m_SendBuffer.Initialize(400);

		// start comlayer
		m_pEventMgr = CreateEventManager(1024 * 1024);
		if (!m_pEventMgr)
			break;

		m_pEventMgr->SetEventArrivedCallback(this, 0);
		m_pComLayer = CreateCommunicationLayer(m_pEventMgr);
		if (!m_pComLayer)
			break;

		if (!m_pComLayer->Startup(m_params.dwLocalInterface,
				m_params.params.dwCmdIpAddress, m_params.params.wCmdPort))
			break;

		//Add Command handler
		m_phCommand = new CCommandPacketHandler(m_pComLayer);
		if (!m_phCommand)
			break;
		AddPacketHandler(m_phCommand);

		//Add Block handler
		m_pBlockPacketHandler = new CBlockPacketHandler(m_pComLayer);
		if (!m_pBlockPacketHandler)
			break;
		AddPacketHandler(m_pBlockPacketHandler);

		m_pSBDAssembler = RMCreateFrameAssembler(2 * 1024 * 1024, this, 0);
		if (!m_pSBDAssembler)
			break;

		m_pSBDReceiver = RMCreateReceiver(
				m_pSBDAssembler->GetReceiverCallback(), 0);

		if (!m_pSBDReceiver)
			break;

		if (!m_pSBDReceiver->JoinSession(
				DWIpAddressToString(m_params.dwLocalInterface).data(),
				DWIpAddressToString(m_params.params.dwDataIpAddress).data(),
				m_params.params.wDataPort, m_params.params.fBitrateKbps,
				m_params.params.fWindowDurationSeconds,
				m_params.params.wMaxPayloadSize,
				m_params.params.wNumDataPacketsInGroup))
			break;

		return TRUE;
	} while (TRUE);

	UninitNetWork();

	return FALSE;
}

//这几步之后就把shareboard的网络层架设起来了，可以接收教师端shareboard程序发送过来的网络包，
//同时也可以向教师端shareboard发送网络包。此时android学生端的shareboard还没有起来。

二、打开shareboard
1》教师端打开shareboard后打开一个.sbd文件后新建一个空白的文件后会发送，shareboard画布文件包，SendSetCanvasFilePacket()
PELCSB_SETCANVASFILE_PACKET
2》android学生端接收之后会做相应的处理
void ShareBoardMainWork::OnFrameArrived() {
	BOOL bFlush = FALSE;
	while (1) {
		int cbBlock = 0;

		PBYTE pRecvData = NULL;
		{
			CAutoSectionLock lock(m_RecvBuffer.GetLock());
			pRecvData = (PBYTE) m_RecvBuffer.GetBlock(cbBlock);
		}

		if (pRecvData == NULL) {
			break;
		}

		//the packet is a compress packet
		//parse the compress packet and get the packets
		PBYTE pData = pRecvData;
		int nPacketsize = 0;
		int nPacketCount = 0;
		memcpy(&nPacketCount, pData, sizeof(UINT));
		pData += sizeof(UINT);
		PELCSB_PACKET pPacket = NULL;

		for (int i = 0; i < nPacketCount; i++) {
			nPacketsize = *((UINT*) pData);
			pData += sizeof(UINT);

			pPacket = (PELCSB_PACKET) pData;
			pData += nPacketsize;

			//if the packet is not self product paket
			//then put it in the canvas
			if (pPacket->dwIpAddress != m_params.dwLocalInterface) {
				JNICTJNativeNotify(NEV_SHAREBOARD_PACKET, nPacketsize,
						(BYTE*) pPacket);
				bFlush = TRUE;
			} else if (pPacket->bFinish == TRUE) {
				//if the packet is self packet and if it's a finsh graph
				JNICTJNativeNotify(NEV_SHAREBOARD_PACKET, nPacketsize,
						(BYTE*) pPacket);
				bFlush = TRUE;
			}

			if (PT_CANVAS_OPTION == pPacket->dwMagic) {
				PELCSB_CANVAS_OPTION_PACKET cavsOptPacket =
						(PELCSB_CANVAS_OPTION_PACKET) pPacket->abParams;
				if (DATA_SETCANVASFILE == cavsOptPacket->dwType) {	//如果是打开画布文件的网络包，则向java层发送打开shareboard的消息，进入点
					JNICTJNativeNotify(NEV_FUNCTION_SHAREBOARD_START,
							nPacketsize, (BYTE*) pPacket);
				}
			}
		}

		{
			CAutoSectionLock lock(m_RecvBuffer.GetLock());
			m_RecvBuffer.PutFreeBlock(pRecvData, cbBlock);
		}
	}

	if (bFlush) {
		//m_DrawFrame.FlushDraw();
		JNICTJNativeNotify(NEV_SHAREBOARD_FLUSH_DRAW, NULL, NULL);
	}
}
2》打开java端shareboard的界面
在ShareBoardJNIBinder.java的文件JNICTJNativeNotify函数中
public void JNICTJNativeNotify(int type, byte[] data) {
	ByteBuffer buffer = ByteBuffer.wrap(data);

	switch (NATIVE_EVENT.values()[type]) {
	case NEV_FUNCTION_SHAREBOARD_START: {

		ELCSB_PACKET sbPacket = new ELCSB_PACKET();
		sbPacket.read(buffer);
		ELCSB_SETCANVASFILE_PACKET setCanvasFilePacket = sbPacket.moptionPacket.mSetCanvasFilePacket;

		if (setCanvasFilePacket.mnCommandNo < mCanvas.getCommandNo()) {
			return;
		}

		if (ClsStatusAfterLogin.InShareBoard != mService
				.getStatusAfterLogin()) {

			sigShareBoardStart.emit();//进入点
		}

		break;
	}

3》响应信号sigShareBoardStart，在NetWorkService.java文件的slotShareBoardStart中
public void slotShareBoardStart()
{
	
	setStatusAfterLogin(ClsStatusAfterLogin.InShareBoard);//将shareboard的界面置顶
	sigShareBoardStart.emit();//进入点
	frontActivity();
}

4》响应信号sigShareBoardStart，在MainActivity.java文件的slotShareBoardStart中
public void slotShareBoardStart() {
	safelyRestoreUi();
}

//至此一个完整的shareboard初始化打开的过程就完成了。

接下来讲一下关闭的流程
1》教师端点击关闭shareboard时回向所有的学生端发送 CMD_STOP_ALL命令，学生端收到后会做相应的处理
在函数void CStudentMainWork::ProcessMemberAction(PACTION_ITEM pItem)中
case CMD_STOP_ALL:
{
	StopAll();
}
break;

2》在stopall中：
void CStudentMainWork::StopAll()
{
//............
//............
	if (m_memberInfo.dwState & CMD_SHAREBOARD) {
		m_memberInfo.dwState &= ~CMD_SHAREBOARD;
		StopShareBoardPassive();//进入点
	}
//...........
//...........
}

3》在StopShareBoardPassive()函数中
void CStudentMainWork::StopShareBoardPassive() {
	ShareBoard_DestroyClient();//进入点
	LogDebug("jni", "CStudentMainWork::StopShareBoardPassive");
}

4》析构掉网络层
BOOL ShareBoard_DestroyClient() {
	LogDebug("jni", "ShareBoard_DestroyClient");

	if (g_pShareBoardMainWork) {
		g_pShareBoardMainWork->UninitJNI();
		delete g_pShareBoardMainWork;
		g_pShareBoardMainWork = NULL;
	}
}
5》向java层发送结束的消息
void ShareBoardMainWork::UninitJNI() {
	JNICTJReleaseTimer(ID_TIMER_NEEDFEEDBACK);
	JNICTJReleaseTimer(ID_TIMER_RESTORE_SHARE_STATE);

	JNICTJNativeNotify(NEV_FUNCTION_SHAREBOARD_STOP, 0, NULL);//向java层发送消息

	UninitNetWork();

	m_jobj_ShareBoardJNIBinder = NULL;
	m_jmethodID_JNICTJNativeNotify = NULL;

	m_pJVM = NULL;

	if (NULL != m_pThreadSendPacket) {
		const char *pEndMark = { "stopshareboard" };
		ssize_t size = write(m_PipeEndThread[1], pEndMark, strlen(pEndMark));
		pthread_join(*m_pThreadSendPacket, NULL);

		delete m_pThreadSendPacket;
		m_pThreadSendPacket = NULL;
	}

	if (0 != m_PipeSendData[0]) {
		close(m_PipeSendData[0]);
		m_PipeSendData[0] = 0;
	}

	if (0 != m_PipeSendData[1]) {
		close(m_PipeSendData[1]);
		m_PipeSendData[1] = 0;
	}

	if (0 != m_PipeEndThread[0]) {
		close(m_PipeEndThread[0]);
		m_PipeEndThread[0] = 0;
	}

	if (0 != m_PipeEndThread[1]) {
		close(m_PipeEndThread[1]);
		m_PipeEndThread[1] = 0;
	}
}
6》
public void JNICTJNativeNotify(int type, byte[] data) {
//............
//............
	case NEV_FUNCTION_SHAREBOARD_STOP: {
		sigShareBoardStop.emit();//进入点
		mCanvas.reset();
		resetShareboardParams();
		break;
	}
//............
//............
}
7》响应信号sigShareBoardStop， NetworkService.java中
public void slotShareBoardStop()
{
	setStatusAfterLogin(ClsStatusAfterLogin.HomeLogged);
	sigShareBoardStop.emit();//进入点
}

8》响应信号sigShareBoardStop， 在MainActivity.java
public void slotShareBoardStop() {
	safelyRestoreUi();
	mFrmShareBoard.resetShareBoard();
}

//至此结束shareboard完成。

