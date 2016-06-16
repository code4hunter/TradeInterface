// clientTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "kcbpcli.h"
#include "kdstrapi.h"

//clienttest -UKCXP00 -P888888 -SKCBP01 -I10.103.3.161 -Nreq_zb -Vans_zb -O21000
//Program:410100
//Parameter:Col1:Val1,Col2:Val2
void GetCmd(char *szCmd, int nBuffLen)
{
	char ch;
	int  i;
	for( i = 0; (i < nBuffLen) &&  ((ch = getchar()) != EOF) 
		&& (ch != '\n'); i++ )
		szCmd[i] = (char)ch;
	szCmd[i] = 0x00;
}

int main(int _argc, char* _argv[])
{
	int  i, nRows;
	char szUserName[64];
	char szPassword[64];	
	char szTmpbuf[1024];
	int  nReturnCode;
	int  nCol;
	int  nErrCode;
	char szErrMsg[256];
	char szProgram[256];
	char szParameter[1024];
	char szColName[64];
	char szColVal[512];
	KCBPCLIHANDLE hHandle;
    tagKCBPConnectOption stKCBPConnection;

	memset( szTmpbuf, 0x00, sizeof(szTmpbuf) );
	memset(&stKCBPConnection, 0 , sizeof(stKCBPConnection));
	
	strcpy(szUserName, "KCXP00");
	strcpy(szPassword, "888888");

	printf( "Copyright (C), 2002-2008, KINGDOM Co., Ltd.\n" );
	printf( "Application name: clienttest.exe\n" );
	printf( "Author: RDC-ZHANGZ Version: 2.0.0.0 Date: 20060405\n" );
	printf( "Description: LBM test tool\n\n" );
		
	strcpy(stKCBPConnection.szServerName, "RDC-ZHANGZ");
	stKCBPConnection.nProtocal = 0;
	strcpy(stKCBPConnection.szAddress, "127.0.0.1");
	stKCBPConnection.nPort = 21000;
	strcpy(stKCBPConnection.szSendQName, "req1");
	strcpy(stKCBPConnection.szReceiveQName, "ans1");

	for(i = 1; i < _argc; i ++) 
	{
		if(memicmp(_argv[i], "-U", 2) == 0 || memicmp(_argv[i], "/U", 2) == 0)
			strncpy(szUserName, _argv[i] + 2, sizeof(szUserName)-1);
		else if(memicmp(_argv[i], "-P", 2) == 0 || memicmp(_argv[i], "/P", 2) == 0)
			strncpy(szPassword, _argv[i] + 2, sizeof(szPassword)-1 );
		else if(memicmp(_argv[i], "-S", 2) == 0 || memicmp(_argv[i], "/S", 2) == 0)
			strncpy(stKCBPConnection.szServerName, _argv[i] + 2, sizeof(stKCBPConnection.szServerName)-1);
		else if(memicmp(_argv[i], "-I", 2) == 0 || memicmp(_argv[i], "/I", 2) == 0)
			strncpy(stKCBPConnection.szAddress, _argv[i] + 2, sizeof(stKCBPConnection.szAddress)-1);
		else if(memicmp(_argv[i], "-N", 2) == 0 || memicmp(_argv[i], "/N", 2) == 0)
			strncpy(stKCBPConnection.szSendQName, _argv[i] + 2, sizeof(stKCBPConnection.szSendQName)-1);
		else if(memicmp(_argv[i], "-V", 2) == 0 || memicmp(_argv[i], "/V", 2) == 0)
			strncpy(stKCBPConnection.szReceiveQName, _argv[i] + 2, sizeof(stKCBPConnection.szReceiveQName)-1);
		else if(memicmp(_argv[i], "-O", 2) == 0 || memicmp(_argv[i], "/O", 2) == 0)
			stKCBPConnection.nPort =  atol(_argv[i] + 2);
		else if(memicmp(_argv[i], "-HELP", 5) == 0 || memicmp(_argv[i], "/HELP", 5) == 0)
		{
			printf("Usage:\nClientTest -Uusername -Ppassword -Sservername -Iip -NsendQName -VreceiveQName -Oport");
			getchar();
			return 0;
		}
	}
	
	/*新建KCBP实例*/
	if( KCBPCLI_Init( &hHandle ) != 0 )
	{
		printf( "KCBPCLI_Init error\n" );
		getchar();
		return -1;
	}

	/*连接KCBP服务器*/
	if( KCBPCLI_SetConnectOption( hHandle, stKCBPConnection ) )
	{
		printf( "KCBPCLI_SetConnectOption error\n" );
		KCBPCLI_Exit( hHandle );
		getchar();
		return -1;
	}

	if( KCBPCLI_SetCliTimeOut( hHandle, 60 ) )
	{
		printf( "KCBPCLI_SetCliTimeOut Error\n" );
		KCBPCLI_Exit( hHandle );
		getchar();
		return -1;
	}

	printf("Connecting %s:%d, ReqQ:%s, AnsQ:%s ... ...\n",
		stKCBPConnection.szAddress, stKCBPConnection.nPort, 
		stKCBPConnection.szSendQName, stKCBPConnection.szReceiveQName );

	nReturnCode = KCBPCLI_SQLConnect( hHandle, stKCBPConnection.szServerName, szUserName, szPassword );
	if(nReturnCode)
	{
		printf("Connect Failure ErrCode= %d\n", nReturnCode);
		getchar();
		return(0);
	}
	printf("Connect success!\n\n");

	while(true)
	{
		memset(szProgram, 0x00, sizeof(szProgram));

		printf("Program:");
		GetCmd(szProgram, sizeof(szProgram)-1);
        if(szProgram[0]==0x00)
			continue;
		if(!strcmp(szProgram,"exit")||!strcmp(szProgram,"quit"))
			break;

		printf("Parameter:");
		GetCmd(szParameter, sizeof(szParameter)-1);
		if(!strcmp(szParameter,"exit")||!strcmp(szParameter,"quit"))
			break;
		
		/*初始化传入参数缓冲区*/
		nReturnCode = KCBPCLI_BeginWrite(hHandle);
		if( nReturnCode !=0 ) 
		{
			printf("KCBPCLI_BeginWrite ErrCode=%d ErrMsg=%s\n", nErrCode, szErrMsg);
			continue;
		}
		
		for(i = 0; !getsubstr(szTmpbuf, sizeof(szTmpbuf)-1, szParameter, i, ','); i++)
		{
			getsubstr(szColName, sizeof(szColName)-1, szTmpbuf, 0, ':');
			getsubstr(szColVal,  sizeof(szColVal)-1,  szTmpbuf, 1, ':');

			if(strcmp(szColName, "orgid")!=0)
			{
				if( KCBPCLI_SetSystemParam(hHandle, KCBP_PARAM_RESERVED, szColVal) )   //必须设置营业部代码
				{
					printf( "KCBPCLI_SetSystemParam error\n" );
					getchar();
					continue;
				}
			}

			KCBPCLI_SetValue(hHandle, szColName, szColVal);
		}
		
		/*客户端向服务器提交请求*/
		nReturnCode  = KCBPCLI_SQLExecute(hHandle, szProgram);
		if( nReturnCode !=0 )
		{
			printf("SqlExecute Program %s Failure ErrCode=%d\n", szProgram, nReturnCode );
			if( nReturnCode == 2003 || nReturnCode == 2004
				|| nReturnCode == 2055 || nReturnCode == 2054 )
			{
				KCBPCLI_SQLDisconnect(hHandle);
				nReturnCode = KCBPCLI_SQLConnect( hHandle, stKCBPConnection.szServerName, szUserName, szPassword );
			}
			continue;
		}
			
		nErrCode = 0;
		KCBPCLI_GetErrorCode(hHandle, &nErrCode);
		if( nErrCode != 0 )
		{
			KCBPCLI_GetErrorMsg( hHandle, szErrMsg );
			printf( "ErrCode:%d, ErrMsg:%s\n", nErrCode, szErrMsg );
			continue;
		}

		nErrCode = KCBPCLI_RsOpen(hHandle);
		if( nErrCode != 0 && nErrCode != 100 )
		{
			printf("KCBPCLI_RsOpen ErrCode=%d\n", nErrCode);
			continue;
		}

		KCBPCLI_SQLNumResultCols(hHandle, &nCol);
    
		KCBPCLI_SQLFetch(hHandle);

		if( KCBPCLI_RsGetColByName( hHandle, "CODE", szTmpbuf ) )
		{
			printf( "Get CODE Fail\n" );
			continue;
		}
		
		if( strcmp(szTmpbuf, "0") != 0 )
		{
			printf( "error code :%s ", szTmpbuf );
			KCBPCLI_RsGetColByName( hHandle, "LEVEL", szTmpbuf );
			printf( "error level :%s ", szTmpbuf );
			KCBPCLI_RsGetColByName( hHandle, "MSG", szTmpbuf );
			printf( "error msg :%s\n", szTmpbuf );
			continue;
		}
		
		nRows = 0;
		if( KCBPCLI_SQLMoreResults(hHandle) == 0 )
		{		
			KCBPCLI_SQLNumResultCols(hHandle, &nCol);
			while( !KCBPCLI_RsFetchRow(hHandle) )
			{
				for( i = 1; i <= nCol; i++ )
				{
					KCBPCLI_RsGetColName( hHandle, i, szTmpbuf, sizeof(szTmpbuf)-1 );
					printf( "%s", szTmpbuf );
					KCBPCLI_RsGetColByName( hHandle, szTmpbuf, szTmpbuf );
					printf( "=%s;", szTmpbuf );
				}
				printf( "\n" );
				nRows++;
			}
		}
		
		printf("Rows = %d\n", nRows);
		KCBPCLI_SQLCloseCursor(hHandle);
	}

	/*断开KCBP连接*/
	KCBPCLI_SQLDisconnect(hHandle);
	KCBPCLI_Exit( hHandle );
	
	return 0;
}

