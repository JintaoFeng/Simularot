#include <vxworks.h>
#include <vme.h>
#include <taskLib.h>

#include <sockLib.h>
#include <ioLib.h>
#include <inetLib.h>
#include <logLib.h>
#include <string.h>
#include <fioLib.h>
#include <stdio.h>
#include <memLib.h>
#include <stdLib.h>
#include <semLib.h>
#include <netinet\\tcp.h>

#include "ComWithWin.h"	//
#include "CMD.h"		//GetCMD
#include "StateMachine.h"	//
#include "TrajHandler.h"	//
#include "ConParamHandler.h"	//
#include "main.h"
#include "N1225A.h"
#include "CMD.h"

/***********************


*************************/

#define LOCAL_SERVER_PORT 1100 /* �����Ķ˿ں� ,�������⣬ֵ̫С*/

//define LOCAL_SERVER_PORT 60000 /* �����Ķ˿ں� ,�������⣬ֵ̫С*/

int g_iListenSkt;
int g_iCommuSkt; 
int iSockAddrSize;
struct	sockaddr_in  sServerAddr;
struct	sockaddr_in  sClientAddr;
int sendBuf=400*1024;

int g_bConnect=0;//�ж������Ƿ�����

/***********************


*************************/

ComData comData;
RecvData recvData;
RXData_t rxData;


int InitNet()
{
	
	if(ERROR ==(g_iListenSkt = socket(AF_INET, SOCK_STREAM, 0)))
	{
		
		printf("Can not open listen socket\n"  );	
		return 1;
	}
 	else
	{
		printf("Listen socket create successfully!\n" );	
	}
	iSockAddrSize = sizeof (struct sockaddr_in);
	bzero ((char *) &sServerAddr, iSockAddrSize);
	sServerAddr.sin_family = AF_INET;
	sServerAddr.sin_len = (u_char) iSockAddrSize;
	sServerAddr.sin_port = htons (LOCAL_SERVER_PORT);
	sServerAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	//���޴���������bind()����0������Ļ���������-1��Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣
	if(ERROR==(bind(g_iListenSkt, (struct sockaddr *) &sServerAddr,iSockAddrSize)))
	{
		
		printf("Unable to bind to port %d\n",LOCAL_SERVER_PORT );	
		close(g_iListenSkt);
		return 1;
	}
	else
	{
		printf("Bind to port successfully!\n" ); 
	}
	
	//���޴���������listen()����0������Ļ�������-1��Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣
	if(ERROR == (listen (g_iListenSkt, 1)))
	{
		printf("Can not listen to listen socket\n");
		close(g_iListenSkt);
		return 1;
	}
	else
	{		
		printf("Listening!\n");	
	}
//	setsockopt(g_iListenSkt,SOL_SOCKET,SO_SNDBUF,(const char*)&sendBuf,sizeof(int));
//	setsockopt(g_iCommuSkt,SOL_SOCKET,SO_SNDBUF,(const char*)&sendBuf,sizeof(int));
	return 0;

}
void ExitNet()
{
	int iRet=0;

	if(g_iCommuSkt==0)
	{
		iRet=close(g_iCommuSkt);	
		printf("%d\n",iRet);
	}

	
	iRet=close(g_iListenSkt);		
	printf("%d\n",iRet);	


}


void RecvFromWin()
{
	int read_rc = 0;
	
	while(1)
	{
		g_iCommuSkt=accept(g_iListenSkt, (struct sockaddr*)(&sClientAddr), &iSockAddrSize);
		g_bConnect=1;
//		setsockopt(g_iCommuSkt,SOL_SOCKET,SO_SNDBUF,(const char*)&sendBuf,sizeof(int));
		logMsg("accept!\n",0,0,0,0,0,0);
		while(1)
		{
			read_rc = recv(g_iCommuSkt,(char *)(&rxData),sizeof(rxData),0);
			
			if(read_rc <= 0)
			{
				g_bConnect=0;
				printf("net end!\n");
				break;
			}
	//		else if(read_rc!=sizeof(rxData))
	//		{
	//			continue;
	//		}
			printf("CMD:%d\n",rxData.iCMD);
//			printf("command: %d\n",rxData.axis);
			printf("axis:%d\n",rxData.axis);
			printf("rxData.iReserved[0]:%d\n",rxData.iReserved[0]);
			printf("rxData.iReserved[1]:%d\n",rxData.iReserved[1]);
			printf("rxData.dParamData[0]:%f\n",rxData.dParamData[0]);
			printf("rxData.dParamData[1]:%f\n",rxData.dParamData[1]);
//			printf("jerk:%f\n",rxData.traj[rxData.axis].jerk);
	//		printf("snap:%f\n",rxData.traj[rxData.axis].snap);
	//		commFun[rxData.command].fun(&rxData);
	//		printf("%d\n%d\n%f\n%f\n%f\n",rxData.axis,rxData.commend,
	//		rxData.controller[rxData.axis].pid.kp,rxData.controller[rxData.axis].pid.ki,rxData.controller[rxData.axis].pid.kd);
	/*		logMsg("RevNumber is %d\n",recvData.iCMD,0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.stageType,0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.iDof,0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.paramType,0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.iOrder,0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.iReserved[0],0,0,0,0,0);
			logMsg("RevNumber is %d\n",recvData.iReserved[4],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[0],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[1],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[2],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[3],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[4],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[5],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[6],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[7],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[8],0,0,0,0,0);
			printf("%f\n",recvData.dParamData[9],0,0,0,0,0);
*/
			GetCMD(&rxData.iCMD);	
		}
	}
}

void SendToWin()
{
	int ret = 0;
	static size=sizeof(comData);
	while(1)
	{
		semTake(TransDataToWin_SemId,WAIT_FOREVER);	
		taskLock();
		if(g_bConnect)
		{
			//��������£�����buff��iCMD����䣬��ʾ��ʾ����֡�������䣬��������֡
			//DataFillIn()����δʵ�֣�������Ҳ���Խ������ݵ���ϣ����buff�����Ļ�
			if(5001==comData.iCMD)
			{
	//			GetParam(); 
			}
			ret = send(g_iCommuSkt,(char *)(&comData),size,0);
		}
		taskUnlock();
	}
}
