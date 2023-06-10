#ifndef MagneticFieldDipoleExtraParameters_hh
#define MagneticFieldDipoleExtraParameters_hh

#include "TsVMagneticField.hh"

class MagneticFieldDipoleExtraParameters : public TsVMagneticField
{
public:
	MagneticFieldDipoleExtraParameters(TsParameterManager* pM, TsGeometryManager* gM,
						  TsVGeometryComponent* component);
	~MagneticFieldDipoleExtraParameters();
	
	void GetFieldValue(const double p[3], double* Field) const;
	void ResolveParameters();

};

#endif
