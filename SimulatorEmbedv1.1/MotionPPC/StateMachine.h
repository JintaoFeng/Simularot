#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include "EnumDefines.h"
#include "controller.h"
#include "TrajGen_4OrderPoly.h"

//#include "CMD.h"
#define INT_FINE_NUM         49
#define INT_COARSE_NUM       50
#define DOUBLE_FINE_NUM         80
#define DOUBLE_COARSE_NUM       200

typedef struct tagDAStruct
{
	int iCoilDA;//���DA
	int iOffset;//��Ȧƫ��
	int iLimit;//��Ȧ�޷�
}DAStruct;

typedef struct tagPosInfor
{
	double dActPos;
	double dSetPoint;
	double dError;
}PosInfor;
typedef unsigned int MOTOR_FLAG;

#define MOTOR_ENABLE    0X0001
#define MOTOR_ACTIVE    0X0002
#define MOTOR_INPOS     0X0004
#define MOTOR_ERROR     0X0008
#define MOTOR_MOVING	0X0010

#define SET_MOTOR_INPOS_FLAG(X)		(X)|=MOTOR_INPOS
#define CLEAR_MOTOR_INPOS_FLAG(X)	(X)&=~MOTOR_INPOS
#define GET_MOTOR_INPOS_FLAG(X)		((X)&MOTOR_INPOS ? 1:0)

#define SET_MOTOR_ENABLE_FLAG(X)		(X)|=MOTOR_ENABLE
#define CLEAR_MOTOR_ENABLE_FLAG(X)		(X)&=~MOTOR_ENABLE
#define GET_MOTOR_ENABLE_FLAG(X)		((X)&MOTOR_ENABLE ? 1:0)

#define SET_MOTOR_MOVING_FLAG(X)		(X)|=MOTOR_MOVING
#define CLEAR_MOTOR_MOVING_FLAG(X)		(X)&=~MOTOR_MOVING
#define GET_MOTOR_MOVING_FLAG(X)		((X)&MOTOR_MOVING ? 1:0)


typedef struct tagAxis	//X�ᣬY��ȵ�
{
	double dActPos;
	double dSetPoint;
	double dError;

	double lastActPos;
	double fVel;
	double fAcc;
	double lastFVel;
	PosInfor posInfor;
	int vError;

	double dSolveMethodOne;
	double dSolveMethodTwo;
	double dActPosNotInUse;

	double dAffData;
	double dConCalc;
	double dFeedForwardCoef;

	double SingleFeedForward_x;
	double SingleFeedForward_x_Switch;
	double SingleFeedForward_y;
	double SingleFeedForward_y_Switch;

	int bIdent;	
	double dIdentGain;

	int bHome;
	int bIntr;
	int bIntrEnable;
	double dHomePos;
	int iHomeCounter;
	int bSwitch;
	int iAxisErrCheck;

	double dStepDis;//Ŀǰδ���ֱջ���ʽ
	double dJogSpd;//Ŀǰδ���ֱջ���ʽ

	PID sPID; 
	PID sPID_Switch;

	LLC sLLC;
	LLC sLLC_Switch;

	LLC sFilter;

	AxisCMD axisCMD;//���ɶ�ָ��
	AxisStatus axisStatus;//���ɶ�״̬
	CloseCMD closeCMD;//���ɶ��л�ָ��
	CloseStatus closeStatus;//���ɶ��л�״ָ̬��

	ControllerType controllerType;//������ʹ��ѡ��
	ControllerType LaserControllerType;//������������ʹ��ѡ��
	double (*OutCalc)();//����������ָ��

	TrajMode trajMode;//�켣��������ģʽö��
	TrajType trajType;//�켣����ö��
	TrajStatus trajStatus;//���ɶ��߹켣״̬
	int (*TrajSine_Poly)(s_TrajParam_Sine_Poly*,double*,double*,double*);//�켣����ָ��

	//int TrajGen_4_Order_Poly(s_TrajParam_4_Order_Poly *TP, double *pd, double *pj, double *pa, double *pv, double *ps, int *c)

	int (*TrajGen4Order)(s_TrajParam_4_Order_Poly *TP, double *pd, double *pj, double *pa, double *pv, double *ps, int *c);

	////////////////////�켣����//////////////////////////////

	s_TrajParam_4_Order_Poly sTP_Run;
	s_TrajParam_Sine_Poly sine_Run;

	double snap;	//��ʾ�ӼӼ��ٶ�
	double jerk;	//��ʾ�Ӽ��ٶ�
	double acc;		//��ʾ���ٶ�
	double vel;		//��ʾ�ٶ�
	double shift;	//��ʾλ��
	int cTrig;		//�켣��ʼ��־λ

	double targetPos;
	double targetVel;
	double targetAcc;
	double targetJerk;
	double targetSnap;

	struct LLC controlObject;


	int iExpoType;//�ع�켣���ͣ������������ع����ͽ���ѡ��
	
	int iTrajStartPoint;//���ڼ�¼�켣��ʼ��λ��
	int iTrajRealTimeFlag;//���ڽ����¼�켣��ʼ��λ�õ�flag
	
	double bHomeDone;//yht
	double stepHome;//yht
	double HomeFlagCorse;
	int iDir;
	int iLaserDir;

	int closeFlag;
	int trajUpdate;
	MOTOR_FLAG flag;
	double posOffset;
	double setFPos;
}Axis;//���ɶ�

typedef struct tagStage
{
	Axis arrAxis[6];
	
	DAStruct DA_Struct[4];

	double dDALimit;
	
	double dConDist[8];
	
	int iADC[8];
	int iDAC[8];//yht
	double dInputVol[8];
	float DAOffset[8];
	
	double Filter2order_b[12][3];
	double Filter2order_a[12][3];
	double Filter2orderOutput[12][3];
	double Filter2orderInput[12][3];
	int Filter2orderFlag[12];

	int iLaser[13];
	int iCounter[6];
	int iSenor[8];
	int laserDCPowerLevel[11];
	int laserACPowerLevel[11];

	int iLaserAfterFilter[8];

	StageCMD stageCMD;//̨��ָ��
	StageStatus stageStatus;//̨��״̬
	StageType stageType;
 
	s_TrajParam_4_Order_Poly sTP_Expo[10];//�ع�켣
	int FieldTransEnable;
	void (*Traj)();

	char TrajQueue[10];	
	double TrajParam[10];

	
	int iStageErrCheck;
	char DDX_Flag;
	char DDY_Flag;
	
}Stage;//̨��(��ģ̨���Ƭ̨)

typedef struct tagMachine
{
	Stage coarseStage;
	Stage fineStage;
	int iExecTime;
	int iTime1;
	int iTime2;
	int iFreq;
	int iServoTime;
	int iServoCnt;
	
	int stepFlagXQ;
	int stepFlagYQ;

	int stepFlagXP;
	int stepFlagYP;
	
	int iTimeHomeYQ;
	int iTimeHomeYP;
	int iTimeHomeXP;
	int iTimeHomeXQ;
	double dIdentData;
	
	/*int stepHome;//yht
	int HomeFlagCorse;*/

	int fineStopFlag;
	int coarseStopFlag;

}MACHINE;//����

typedef struct tagTransConParam
{
	int iCmdID;
	StageType stageType;
	int iDof;
	ParamType  paramType;
	int iOrder;
	double dParamter[4][24];
}TransConParam;

typedef struct 
{
    float kp;
    float ki;
    float kd;
}PID_t;

typedef struct  
{
	PID_t pid;
    float feedForwardCoff;
}controller_t;

typedef struct tagComData
{
	int iCMD;
	int iFineData[INT_FINE_NUM];
	int iCoarseData[INT_COARSE_NUM];
	double dFineData[DOUBLE_FINE_NUM];
	double dCoarseData[DOUBLE_COARSE_NUM];
	TransConParam transConParam;
	double refPosition[10];
	double refVel[10];
	double refAcc[10];
	double refJerk[10];
	double refSnap[10];
	double feedPosition[10];
	double feedVel[10];
	double feedAcc[10];
	double feedJerk[10];
	double feedSnap[10];
	double Error[10];
	double conDist[10];
	float DAOffset[10];
	int axisStatus[10];
	int stageStatus[10];
	int dir[10];
	int laserACPowerLevel[10];
	int laserDCPowerLevel[10];
	int motorState[10];
	controller_t controller[10]; 
}ComData;

typedef struct 
{
    double pos;
    double vel;
    double acc;
    double jerk;
    double snap;
}traj_t;

typedef struct tagRecvData
{
	int iCMD;
    int axis;
 /*   command_e command;
    controller_t controller[10];
    traj_t traj[10];
    double jogSpeed[10];*/
//	StageType stageType;
//	int iDof;
//	ParamType  paramType;
//	int iOrder;
	int iReserved[8];
	double dParamData[30];	
//	TransConParam transConParam;
}RXData_t;

typedef struct 
{
    int seq;
    int axis;
    command_e command;
    controller_t controller[10];
    traj_t traj[10];
}RecvData;

extern RecvData recvData;
extern ComData comData;
extern MACHINE Machine;
extern RXData_t rxData;

void abc(RXData_t A);
void setPID_Kp(int axis,float kp);
void setPID_Ki(int axis,float ki);
void setPID_Kd(int axis,float kd);
//void setFeedForwardCoff(int axis,float value);
void setTrajPos(int axis,double pos);
void setTrajVel(int axis,double vel);
void setTrajAcc(int axis, double acc);
void setTrajJerk(int axis,double jerk);
void setTrajSnap(int axis,double snap);
extern void AxisStateMachine(Axis* pAxis,int iAxisNum);
extern void StageStateMachine(Stage * pStage);
extern void SwitchStateMachine(Axis* pAxis,int iAxisNum);

#endif
