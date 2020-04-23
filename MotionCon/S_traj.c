#include "S_traj.h"
#include <math.h>

int sign(double pos)
{
	if(pos>0)
		return 1; 
    else if(pos == 0.0)
		return 0;
	else
		return -1;
}

void paramInit(S_Traj_t *traj)
{
	if(traj->vamx<=0||traj->amax<=0||traj->jerk<=0)
	{
		logMsg("��������С�ڵ���0\n",0,0,0,0,0,0);
		return;
	}
	traj->sigma=sign(traj->pos);
	traj->v0=traj->sigma*traj->v0;
	traj->v1=traj->sigma*traj->v1;
	
}

void calcTrajParam(S_Traj_t *traj)
{
	//���жϵ�ǰλ���ܷ�������ٶȵ�ĩ�ٶ�
	double T1,T2,Tjs;
	double delta;
	float gama=0.99;
	T1=sqrt(fabs(traj->v1-traj->v0)/traj->jerk);
	T2=traj->acc/traj->jerk;
	Tjs=MIN(T1,T2);
	if(T1<T2)
	{
		if(traj->pos<Tjs*(traj->v0+traj->v1))
		{
			logMsg("λ�ƹ�С�������������ĩ�ٶ�\n",0,0,0,0,0,0);
			return;
		}
	}
	else
	{
		if(traj->pos<0.5*(traj->v0+traj->v1)*(Tjs+fabs(traj->v1-traj->v0)/traj->acc))
		{
			logMsg("λ�ƹ�С�������������ĩ�ٶ�\n",0,0,0,0,0,0);
			return;
		}
	}

	
	//��������
	if((traj->vel-traj->v0)*traj->jerk<pow(traj->acc,2))
	{
		//�ﲻ���趨��acc
		traj->Tj1=sqrt((traj->vel-traj->v0)/traj->jerk);
		traj->Ta=2*traj->Tj1;
		traj->alima=traj->jerk*traj->Tj1;
	}
	else
	{
		//���Դﵽamax
		traj->Tj1=traj->acc/traj->jerk;
		traj->Ta=traj->Tj1+(traj->vel-traj->v0)/traj->acc;
		traj->alima=traj->acc;
	}

	if((traj->vel-traj->v1)*traj->jerk<pow(traj->acc,2))
	{
		//�ﲻ���趨��acc
		traj->Tj2=sqrt((traj->vel-traj->v1)/traj->jerk);
		traj->Td=2*traj->Tj2;
		traj->alimd=-1*traj->jerk*traj->Tj2;
	}
	else
	{
		//���Դﵽamax
		traj->Tj2=traj->acc/traj->jerk;
		traj->Ta=traj->Tj2+(traj->vel-traj->v1)/traj->acc;
		traj->alimd=-traj->acc;
	}




	//�������ٶ�ʱ��
	traj->Tv=traj->pos/traj->vel-traj->Ta*0.5*(1+traj->v0/traj->vel)-traj->Td*0.5*(1+traj->v1/traj->vel);
	if(traj->Tv>=0)
	{
		//�ܴﵽ����ٶ�
		traj->vlim=traj->vel;
		traj->T=traj->Ta+traj->Tv+traj->Td;
		traj->La=traj->v0*traj->Ta+0.5*traj->jerk*traj->Tj1*(2*pow(traj->Tj1,2)+3*traj->Tj1*(traj->Ta-2*traj->Tj1)+pow(traj->Ta-2*traj->Tj1,2));
		traj->Lv=traj->vlim*traj->Tv;
		traj->Ld=traj->vlim*traj->Td-0.5*traj->jerk*traj->Tj2*(2*pow(traj->Tj2,2)+3*traj->Tj1*(traj->Td-2*traj->Tj2)+pow(traj->Td-2*traj->Tj2,2));
		return;
	}
	else if(traj->Tv<0)
	{
		//�ﲻ������ٶ�
		
		traj->Tv=0;
		traj->Tj1=traj->Tj2=traj->acc/traj->jerk;
		delta=pow(traj->acc,4)/pow(traj->jerk,2)+2*(pow(traj->v0,2)+pow(traj->v1,2))+traj->acc*(4*traj->pos-2*traj->acc/traj->jerk(traj->v0+traj->v1));
		traj->Ta=(pow(traj->acc,2)/traj->jerk-2*traj->v0+sqrt(delta))/(2*traj->acc);
		traj->Td=(pow(traj->acc,2)/traj->jerk-2*traj->v1+sqrt(delta))/(2*traj->acc);
		if(traj->Ta>(2*traj->Tj1)&&traj->Td>(2*traj->Tj2))
		{
			//���ٶκͼ��ٶζ��ܴﵽ�����ٶ�
			traj->vlim=traj->v0+(traj->Ta-traj->Tj1)*traj->acc;
			traj->T=traj->Ta+traj->Tv+traj->Td;
			traj->La=traj->v0*traj->Ta+0.5*traj->jerk*traj->Tj1*(2*pow(traj->Tj1,2)+3*traj->Tj1*(traj->Ta-2*traj->Tj1)+pow(traj->Ta-2*traj->Tj1,2));
			traj->Lv=traj->vlim*traj->Tv;
			traj->Ld=traj->vlim*traj->Td-0.5*traj->jerk*traj->Tj2*(2*pow(traj->Tj2,2)+3*traj->Tj1*(traj->Td-2*traj->Tj2)+pow(traj->Td-2*traj->Tj2,2));
			return;
		}
		else
		{
			//������һ�β��ܴﵽ�����ٶ�
			while(traj->Ta<(2*traj->Tj1)||traj->Td<(2*traj->Tj2))
			{
				if(traj->Ta>0 && traj->Td>0)
				{
					traj->acc*=gama;
					traj->Tj1=traj->Tj2=traj->acc/traj->jerk;
					delta=pow(traj->acc,4)/pow(traj->jerk,2)+2*(pow(traj->v0,2)+pow(traj->v1,2))+traj->acc*(4*traj->pos-2*traj->acc/traj->jerk(traj->v0+traj->v1));
					traj->Ta=(pow(traj->acc,2)/traj->jerk-2*traj->v0+sqrt(delta))/(2*traj->acc);
					traj->Td=(pow(traj->acc,2)/traj->jerk-2*traj->v1+sqrt(delta))/(2*traj->acc);
				}
				else
				{
					//����ĳһ�׶�ʱ��С��0
					if(traj->Ta<0)
					{
						traj->Ta=0;
						traj->Tj1=0;
						traj->Td=2*traj->pos/(traj->v0+traj->v1);
						traj->Tj2=(traj->jerk*traj->pos-sqrt(traj->jerk))/();
					}
						else if(traj->Td<0)
				}
			}
		}
	}
}




