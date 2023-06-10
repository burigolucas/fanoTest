// BeginRun for TOPAS

#include "DensityCorrectionToggle.hh"

#include "TsParameterManager.hh"

#include "G4Material.hh"
#include "G4IonisParamMat.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"

DensityCorrectionToggle::DensityCorrectionToggle(TsParameterManager* pM)
{
	if (pM->ParameterExists("Fano/DisableDensityEffectCorrection")) {
		G4String* strMaterials = pM->GetStringVector("Fano/DisableDensityEffectCorrection");
		G4int nbMaterials = pM->GetVectorLength("Fano/DisableDensityEffectCorrection");
		G4NistManager* fNistManager = G4NistManager::Instance();
		for ( G4int ix = 0; ix < nbMaterials; ix++)
		{
			G4cout << "Disabling density effect correction for " << strMaterials[ix] << G4endl;
			G4Material* mat = fNistManager->FindOrBuildMaterial(strMaterials[ix]);
			G4IonisParamMat* ion = mat->GetIonisation();
			ion->SetDensityEffectParameters(0,0,0,0,0,0);
		}
	}
}


DensityCorrectionToggle::~DensityCorrectionToggle()
{
}
