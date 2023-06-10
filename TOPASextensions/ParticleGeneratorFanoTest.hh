#ifndef ParticleGeneratorFanoTest_hh
#define ParticleGeneratorFanoTest_hh

#include "TsVGenerator.hh"

#include "G4VPhysicalVolume.hh"

class G4Navigator;
class G4Material;

class ParticleGeneratorFanoTest : public TsVGenerator
{
public:
	ParticleGeneratorFanoTest(TsParameterManager* pM, TsGeometryManager* gM, TsGeneratorManager* pgM, G4String sourceName);
	~ParticleGeneratorFanoTest();

	void ResolveParameters();
	void UpdateForNewRun(G4bool rebuiltSomeComponents);

	void GeneratePrimaries(G4Event* );
	
private:
	void RetrieveNavigator();
	
	G4bool fRecursivelyIncludeChildren;
	G4Navigator* fNavigator;
	G4double fRefDensity;
	
	std::vector<G4VPhysicalVolume*> fVolumes;

	G4bool fNeedToRetrieveNavigator;

	G4int fGeometryType; // 0 for TsBox, 1 for TsCylinder
	G4int fSpatialDistribution; // 0 for uniform, 1 for Zaxis
	G4bool fIsotropic;
	G4ThreeVector fDirection; // only used if not isotropic

	G4double fXMin;
	G4double fXMax;
	G4double fYMin;
	G4double fYMax;
	G4double fZMin;
	G4double fZMax;

	// dimensions for TsBox
	G4double fHLX;
	G4double fHLY;
	G4double fHLZ;

	// dimensions for TsCylinder
	G4double fRmax;
	G4double fHL;
};
#endif
