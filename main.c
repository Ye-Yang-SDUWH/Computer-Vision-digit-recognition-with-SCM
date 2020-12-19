#include <stc8.h>
#include <math.h>
#include <stdio.h>
void InitUART()
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE0;		//设定定时初值
	TH1 = 0xFE;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES = 1;
	EA = 1;
}
void InitUART2()
{
	S2CON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xE0;		//设定定时初值
	T2H = 0xFE;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	IE2   = ES2;        // 使能串口中断
}
void matrix_multi(int r1,int c1, int r2, int c2,float *a,float *b,float *c)
{
    float sum=0;
    int i=0,j=0,m=0;
    for (i=0;i<r1;i++)
    {
        for (j=0;j<c2;j++)
        {
            sum=0;
            for (m=0;m<r2;m++)
            {
                sum = sum+a[i*r2+m]*b[m*c2+j];
            }
            c[i*r2+j] = sum;
        }
    }
}
float multi(int r1,int c1, int r2, int c2,float *a,float *b)
{
    int i=0,j=0;
    float sum=0;
    for (i=0;i<r1;i++)
    {
        for (j=0;j<c1;j++)
        {
            sum = sum+a[i*r2+j]*b[i*r2+j];
        }
    }
    return sum;
}
float multi_char(int r1,int c1, int r2, int c2,unsigned char *a,float *b)
{
    int i=0,j=0;
    float sum=0;
    for (i=0;i<r1;i++)
    {
        for (j=0;j<c1;j++)
        {
            sum = sum+a[i*r2+j]*b[i*r2+j];
        }
    }
    return sum;
}
void con2D(float *filter,int input_size,float *input,float *output,float bias)
{
    int i=0,j=0,m=0,n=0;
    float mid[3][3]={0};
    for (i=0;i<input_size-2;i++)
    {
        for(j=0;j<input_size-2;j++)
        {
            for (m=0;m<3;m++)
            {
                for (n=0;n<3;n++)
                {
                    mid[m][n]=input[(i+m)*input_size+j+n];
                }
            }
            output[i*(input_size-2)+j]=multi(3,3,3,3,*mid,filter)+bias;
        }
    }
}
void con2D_char(float *filter,int input_size,unsigned char *input,float *output,float bias)
{
    int i=0,j=0,m=0,n=0;
    unsigned char mid[3][3]={0};
    for (i=0;i<input_size-2;i++)
    {
        for(j=0;j<input_size-2;j++)
        {
            for (m=0;m<3;m++)
            {
                for (n=0;n<3;n++)
                {
                    mid[m][n]=input[(i+m)*input_size+j+n];
                }
            }
            output[i*(input_size-2)+j]=multi_char(3,3,3,3,*mid,filter)+bias;
        }
    }
}
void multicon2D(float *filter1,float *filter2,unsigned char input_size,float *input1,float *input2,float *output,float bias)
{
    unsigned char i=0,j=0,m=0,n=0;
    float mid1[3][3]={0};
    float mid2[3][3]={0};
    for (i=0;i<input_size-2;i++)
    {
        for(j=0;j<input_size-2;j++)
        {
            for (m=0;m<3;m++)
            {
                for (n=0;n<3;n++)
                {
                    mid1[m][n]=input1[(i+m)*input_size+j+n];
                    mid2[m][n]=input2[(i+m)*input_size+j+n];
                }
            }
            output[i*(input_size-2)+j]=multi(3,3,3,3,*mid1,filter1)+multi(3,3,3,3,*mid2,filter2)+bias;
        }
    }
}
float mymax(float a,float b,float c,float d)
{
    float result = a;
    if (result<b)
        result = b;
    if (result<c)
        result = c;
    if (result<d)
        result = d;
    return result;
}
float mymax2(float a)
{
    float result = a;
    if (result>0)
        return result;
    else
        return 0;
}
int maxinmatrix(float *input,unsigned char input_size)
{
    float result = input[0];
    int i=0,flag=0;
    for (i=0;i<input_size;i++)
    {
        if (result<input[i])
        {
            result=input[i];
            flag=i;
        }
    }
    return flag;
}
void maxpooling(float *input,float *output,int input_size)
{
    int i=0,j=0;
    float mid[2][2] = {0};
    for (i=0;i<input_size/2;i++)
    {
        for (j=0;j<input_size/2;j++)
        {
            mid[0][0] = input[i*2*input_size+j*2];
            mid[0][1] = input[i*2*input_size+j*2+1];
            mid[1][0] = input[(i*2+1)*input_size+j*2];
            mid[1][1] = input[(i*2+1)*input_size+j*2+1];
            output[i*input_size/2+j] = mymax(mid[0][0],mid[0][1],mid[1][0],mid[1][1]);
        }
    }
}

void flatten(float *output,float *input1,float *input2,unsigned char input_size)
{
    unsigned char i=0,j=0;
    unsigned char position1=0,position2=1;
    for (i=0;i<input_size;i++)
    {
        for (j=0;j<input_size;j++)
        {
            output[position1]=input1[i*input_size+j];
            output[position2]=input2[i*input_size+j];
            position1+=2;
            position2+=2;
        }
    }
}

void relu(float *input,int input_size)
{
    int i=0,j=0;
    for (i=0;i<input_size;i++)
    {
        for (j=0;j<input_size;j++)
        {
            input[i*input_size+j] = mymax2(input[i*input_size+j]);
        }
    }
}
void addbais(float *input,float *bias,unsigned char input_size)
{
    unsigned char i=0;
    for(i=0;i<input_size;i++)
    {
        input[i]+=bias[i];
    }
}
/*
void coutmatrix(float *matrix,unsigned char matrix_size)
{
    int i=0,j=0;
    for (i=0;i<matrix_size;i++)
    {
        for (j=0;j<matrix_size;j++)
        {
            cout<<matrix[i*matrix_size+j]<<'\t';
        }
        cout<<endl;
    }
}*/
unsigned char a=0,b=0,i=0,j=0,result=0;
unsigned char input[28][28]={0};
int main()
{
    float code filter1[3][3]={0.026877893,0.0024490058,-0.0038809474,
                        0.011751894,0.029727688,0.020612525,
                        0.03121622,0.017438903,0.025908789};
    float code bias1 = 0.3787181;
    float output1[26][26]={0};
    float output11[13][13]={0};
    float code filter21[3][3]={0.21841569,0.23534615,0.2698171,
                        -0.3165811,-0.067939274,0.06811985,
                        -0.08856419,-0.19019513,-0.160051550};
    float code filter22[3][3]={0.09412318,-0.07198365,-0.04115128,
                        0.05839803,0.09789113,0.05938016,
                        0.22177614,0.05679257,0.00258056};
    float bias2[2]={-0.6861104,0.6645669};
    float output21[11][11]={0};
    float output22[11][11]={0};
    float output21x[10][10];
    float output22x[10][10];
    float output211[5][5]={0};
    float output221[5][5]={0};
    float filter31[3][3]={0.13849796,0.21144351,0.33562616,
                        0.066516094,-0.023074748,-0.105195604,
                        -0.062457133,-7.472321E-4,-0.03128638};
    float filter32[3][3]={0.09758412,0.0644626,-0.08139924,
                        -0.11684509,-0.293109,0.14427741,
                        0.2796689,0.11797909,0.04161852};
    float filter33[3][3]={0.10588427,0.0266002,0.04106544,
                        0.17510168,-0.08350485,-0.08419501,
                        -0.11508066,-0.22588418,-0.3371619};
    float filter34[3][3]={0.03785353,0.14445034,0.25633082,
                        -0.04890805,0.05961885,0.5158213,
                        -0.09492277,-0.11171421,-0.18987674};
    float bias3[2]={0.5505126,0.80391127};
    float output31[3][3]={0};
    float output32[3][3]={0};
    float output4[18]={0};
    float code filter4[18][10]={-0.6950672,-0.2483055,-0.39549467,-0.3726253,-0.49124247	,-1.020163	,-0.9900969	,-1.1273237	,-1.0625298	,-1.7913771,
                        -0.3064173	,-1.2911744	,-0.67948574	,-0.34682897,0.056328215,-0.41871762,-0.35353506,-0.22631417,-0.75012785,-0.52476084,
                        -0.8710602,-0.070809595	,-0.24819282,-0.56072396,-0.111027464	,-0.4531749	,-0.17972122,-0.65466684,-0.5069217	,-0.96658576,
                        -0.07472159	,0.4947082	,0.05588404	,0.051204767,	-0.7935346,	-0.4630534,	-0.12907185,	-0.30937147	,-0.1818692,	-0.0038314592,
                        -0.73840743	,-1.5128525	,-0.31266168,	-0.505206,	-0.25439256,	-0.41704386,	0.22820906,	-1.7631398,	-0.17938304,	-0.17872135,
                        -0.4009993,	-0.39041874	,-0.49821264,	-0.33696437,	-0.5154629,	-1.0116367,	-1.2263476	,-0.6609925	,-0.530346,	-0.74495983,
                        -1.180767	,-0.21398257,-0.27853727,	0.06601802,	-1.3735504,	-0.7660056	,-1.692614	,-0.053204432,	-1.002366,	-1.1816847,
                        0.083955154	,-0.5725276	,-0.26070765,	-0.86057675,	-0.6528268,	-0.6103497,	0.023674428	,-0.39490524	,-0.22583456	,-0.9334678,
                        -0.28020486	,-0.9319848	,-0.20114966,	-0.6992634	,-0.6636764	,-0.6625817	,-0.60025674	,-0.18318178,	-0.061744798	,-0.19631624,
                        -0.91925037,0.55258226,	-0.34999737,	-0.42411384	,-0.45238423,	-0.07678114,	-0.023236005,	-0.8010969	,0.09230818,	-0.6527934,
                        -0.12636189	,-1.0710901	,-0.5691526,	-0.69808763	,-0.53083235,	0.34122443,	0.25409353,	-0.87609637,	-0.5729688,	-0.6606306,
                        -0.27626264	,-0.76273924,-0.663748,	-0.7747919	,-0.26816106	,-1.1084417	,-0.70757234,	0.14756435,	-0.9666611	,0.2770257,
                        -1.0265214	,-0.5210795	,-0.2202588,	-0.12912355,	-0.27001563	,-0.82823604,	-1.034841,	-0.5515316,	-0.7439878,	-0.77861905,
                        -0.34718528	,0.11478923	,-0.24814917,	-0.48943415,	-0.26117057	,-0.5994344,	-0.72239816	,0.21231042	,-0.17124064	,-0.0703548,
                        -0.021672625	,-0.18148601,-0.19072293,	-0.06359869,	-0.5803027	,-0.24751909,	-0.8728722,	-0.051267724,	-0.57637656,	-0.40444705,
                        -0.72662294,	-0.44748586	,-0.6276443,	-0.5162069,	-0.06636092,	-0.26541027,	-0.5195186	,0.112665094	,-0.26975575	,-0.07799163,
                        -0.1152537	,-0.939387	,-0.6827356,	-0.3859744,	-1.40195	,0.011595812,	-0.039274678,-1.3291535,	0.086109795,-0.75617826,
                        -0.80614877,	-0.9916644	,-0.95972645,	-0.3094521,	-0.17965072,	-0.27655914,	-0.7735558,	-0.52675134,	-0.51521826,-0.33292282};
    float output5[10]={0};
    float bias4[10]={-1.2089835,0.30174118,-1.1874148,-1.1469631,0.8259957,0.78507334,0.42420974,-0.025447499,-0.90321213,0.9864186};
    int result=0;
		InitUART();
		InitUART2();
		while(1)
		{
			if(i==28)
			{
    con2D_char(*filter1,28,*input,*output1,bias1);
    relu(*output1,26);
    maxpooling(*output1,*output11,26);
    con2D(*filter21,13,*output11,*output21,bias2[0]);
    con2D(*filter22,13,*output11,*output22,bias2[1]);
    relu(*output21,11);
    relu(*output22,11);
    for(a=0;a<10;a++)
    {
        for (b=0;b<10;b++)
        {
            output21x[a][b]=output21[a][b];
            output22x[a][b]=output22[a][b];
        }
    }
    maxpooling(*output21x,*output211,10);
    maxpooling(*output22x,*output221,10);
    multicon2D(*filter31,*filter32,5,*output211,*output221,*output31,bias3[0]);
    multicon2D(*filter33,*filter34,5,*output211,*output221,*output32,bias3[1]);
    relu(*output31,3);
    relu(*output32,3);
    flatten(output4,*output31,*output32,3);
    matrix_multi(1,18,18,10,output4,*filter4,output5);
    addbais(output5,bias4,10);
    result = maxinmatrix(output5,10);
		SBUF = result;
		i+=1;
		S2BUF = result;
		
   // printf("%d",result);
    return 0;
	}
}
}



/* UART 中断服务函数 */
void InterruptUART() interrupt 4 using 1
{
 if (RI) //接收到字节
 {
	RI = 0; //手动清零接收中断标志位
  input[i][j] = SBUF ; //接收到的数据保存到接收字节变量中
  //SBUF = input[i][j]; //接收到的数据又直接发回，叫作-"echo"
  //用以提示用户输入的信息是否已正确接收
	j+=1;
	if (j==28)
	 {j=0;i+=1;}
 }
 if (TI) //字节发送完毕
 {
	 TI = 0; //手动清零发送中断标志位
 } 
}
void UART2_Isr() interrupt 8 
{ 
 if (S2CON & 0x02) 
 { 
 S2CON &= ~0x02; //清中断标志
 
 } 
 if (S2CON & 0x01) 
 { 
 S2CON &= ~0x01; //清中断标志
 
 } 
}