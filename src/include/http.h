//===========================================================================
// http.h
//   Copyright (C) 2012 Free Software Foundation, Inc.
//   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
//
//This file is part of DTHAS.
//
//DTHAS is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or 
//(at your option) any later version.
//
//DTHAS is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
//===========================================================================
#ifndef	_X_HTTP_H_
#define	_X_HTTP_H_

//-------------------------------------------------------------------------
// http status
//-------------------------------------------------------------------------
#define	HTTP_NR_STATUS		4

#define	HTTP_STATE_END		-1	// ERROR

#define	HTTP_STATE1		0	// INIT
#define	HTTP_STATE2		1	// ESTABLISH
#define	HTTP_STATE3		2	// WAIT_C
#define	HTTP_STATE4		3	// WAIT_S

//-------------------------------------------------------------------------
// http action in each status
//-------------------------------------------------------------------------
#define	HTTP_NR_ACTION		7

#define	EX_TIME			0	//

#define	SEND_HTTPREQUEST	1	//
#define	SEND_HTTPRESPONSE	2	//
#define	SEND_HTTPCLOSE		3	//

#define	RECV_HTTPREQUEST	4	//
#define	RECV_HTTPRESPONSE	5	//
#define	RECV_HTTPCLOSE		6	//

//-------------------------------------------------------------------------
// http header
//-------------------------------------------------------------------------
struct s_state_code
{
	s8	code[4];
	s8	phrase[28];
};


//-------------------------------------------------------------------------
// http method
//-------------------------------------------------------------------------

#define	NR_METHOD		8		//

#define	METHOD_GET		0		//GET
#define	METHOD_HEAD		1		//HEAD
#define	METHOD_POST		2		//POST
#define	METHOD_PUT		3		//PUT	
#define	METHOD_TRACE		4		//TRACE
#define	METHOD_CONNECT		5		//CONNECT
#define	METHOD_DELETE		6		//DELETE
#define	METHOD_OPTION		7		//OPTION

//-------------------------------------------------------------------------
// http request header name
//-------------------------------------------------------------------------
#define	REQ_USER_AGENT		0		//User-Agent
#define	REQ_ACCEPT		1		//Accept
#define	REQ_ACCEPT_CHARSET	2		//Accept-Charset
#define	REQ_ACCEPT_ENCODING	3		//Accept-Encoding
#define	REQ_ACCEPT_LANGUAGE	4		//Accept-Language
#define	REQ_AUTHORIZATION	5		//Authorization
#define	REQ_HOST		6		//Host
#define	REQ_DATE		7		//Date
#define	REQ_UPGRADE		8		//Upgrade
#define	REQ_COOKIE		9		//Cookie
#define	REQ_IF_MODIFIED_SINCE	10		//If-Modified-Since

//-------------------------------------------------------------------------
// http response header name
//-------------------------------------------------------------------------
#define	RES_DATE		0		//Date
#define	RES_UPGRADE		1		//Upgrade
#define	RES_SERVER		2		//Server
#define	RES_SET_COOKIE		3		//Set-Cookie
#define	RES_CONTENT_ENCODING	4		//Content-Encoding
#define	RES_CONTENT_LANGUAGE	5		//Content-Language
#define	RES_CONTENT_LENGTH	6		//Content-Length
#define	RES_CONTENT_TYPE	7		//Content-Type
#define	RES_LOCATION		8		//Location
#define	RES_ACCEPT_RANGES	9		//Accept-Ranges
#define	RES_LAST_MODIFIED	10		//Last-Modified

//-------------------------------------------------------------------------
// http state code
//-------------------------------------------------------------------------
#define	NR_STS_CODE			18		//

#define	STS_CONTINUE			0		//100
#define	STS_SWITCHING			1		//101
#define	STS_OK				2		//200
#define	STS_CREATED			3		//201
#define	STS_ACCEPTED			4		//202
#define	STS_NO_CONTENT			5		//204
#define	STS_MOVED_PERMANENTLY		6		//301
#define	STS_MOVED_TEMPORARILY		7		//302
#define	STS_NOT_MODIFIED		8		//304
#define	STS_BAD_REQUEST			9		//400
#define	STS_UNAUTHORIZED		10		//401
#define	STS_FORBIDDEN			11		//403
#define	STS_NOT_FOUND			12		//404
#define	STS_METHOD_NOT_ALLOWED		13		//405
#define	STS_NOT_ACCEPTABLE		14		//406
#define	STS_INTERNAL_SERVER_ERROR	15		//500
#define	STS_NOT_IMPLEMENTED		16		//501
#define	STS_SERVICE_UNAVAILABLE		17		//503


PUBLIC s32 	*p_hhttp_state_tab;

PUBLIC s32 	http_state;

PUBLIC u8	*p_hhttp_stack;

PUBLIC	s8*	p_http_method;

PUBLIC struct s_state_code *	p_state_code;

#endif
