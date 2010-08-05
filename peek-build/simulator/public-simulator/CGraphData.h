//	Author		: Gerald Naveen A
//  email		: ageraldnaveen@yahoo.com
//  copyright	: you are free to modify this code and use in your applications
//				  but plz do not remove this header.

//////////////////////////////////////////////////////////////////////
// GraphData.h: interface for the CGraphData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHDATA_H__98FCBEFD_6C06_4605_9DF9_33EAAE04A4FF__INCLUDED_)
#define AFX_GRAPHDATA_H__98FCBEFD_6C06_4605_9DF9_33EAAE04A4FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct _data
{
	long int Value;
	struct _data* Next;
};

class CGraphData  
{
private:
	struct _data *right; // data will be deleted from left and inserted into right
	bool AllOk;
public:
	bool IsAllOk();
	struct _data *left;
	void NewValue(long int value);
	CGraphData(int size);
	virtual ~CGraphData();

};

#endif // !defined(AFX_GRAPHDATA_H__98FCBEFD_6C06_4605_9DF9_33EAAE04A4FF__INCLUDED_)
