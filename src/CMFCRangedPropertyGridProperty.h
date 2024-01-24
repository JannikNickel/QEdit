#pragma once
#include <afxpropertygridctrl.h>
#include <algorithm>

class CMFCRangedPropertyGridProperty : public CMFCPropertyGridProperty
{
protected:
	int32_t min = INT_MIN;
	int32_t max = INT_MAX;

	using CMFCPropertyGridProperty::CMFCPropertyGridProperty;

	BOOL TextToVar(const CString& strText) override
	{
		int32_t newVal;
		if(_stscanf_s(strText, _T("%d"), &newVal) != 1)
		{
			return FALSE;
		}

		newVal = std::clamp(newVal, min, max);
		CString newStr;
		newStr.Format(_T("%d"), newVal);
		return CMFCPropertyGridProperty::TextToVar(newStr);
	}

public:
	void SetRange(int32_t min, int32_t max)
	{
		this->min = min;
		this->max = max;
	}
};
