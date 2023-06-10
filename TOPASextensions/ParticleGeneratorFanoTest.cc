// Particle Generator for FanoTest
//

#include "ParticleGeneratorFanoTest.hh"

#include "TsParameterManager.hh"

#include "TsVGeometryComponent.hh"

#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4VisExtent.hh"
#include "G4Scene.hh"
#include "Randomize.hh"

#include "G4Material.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonisParamMat.hh"

#include "G4RunManager.hh"

//class TsGeometryManager;

ParticleGeneratorFanoTest::ParticleGeneratorFanoTest(TsParameterManager* pM, TsGeometryManager* gM, TsGeneratorManager* pgM, G4String sourceName) :
TsVGenerator(pM, gM, pgM, sourceName), fNeedToRetrieveNavigator(true)
{
	fRecursivelyIncludeChildren = true;
	fSpatialDistribution = 0;
	ResolveParameters();
}


ParticleGeneratorFanoTest::~ParticleGeneratorFanoTest()
{
}


void ParticleGeneratorFanoTest::ResolveParameters() {
	TsVGenerator::ResolveParameters();
	
	G4Material* fRefMaterial = fComponent->GetMaterial(fPm->GetStringParameter(fComponent->GetFullParmName("Material")));
	fRefDensity = fRefMaterial->GetDensity();

	// Check component type
	G4String strCompType = fPm->GetStringParameter(fComponent->GetFullParmName("Type"));
	strCompType.toLower();
	if (strCompType == "tsbox") {
		fGeometryType = 0;
		fHLX = fPm->GetDoubleParameter(fComponent->GetFullParmName("HLX"),"Length");
		fHLY = fPm->GetDoubleParameter(fComponent->GetFullParmName("HLY"),"Length");
		fHLZ = fPm->GetDoubleParameter(fComponent->GetFullParmName("HLZ"),"Length");
	} else if (strCompType == "tscylinder") {
		fGeometryType = 1;
		fRmax = fPm->GetDoubleParameter(fComponent->GetFullParmName("Rmax"),"Length");
		fHL = fPm->GetDoubleParameter(fComponent->GetFullParmName("HL"),"Length");
		fHLZ = fHL;
	} else {
		G4cerr << "Topas is exiting due to a serious error." << G4endl;
		G4cerr << GetName() << " has unsupported component type.." << G4endl;
		G4cerr << "Supported component types are TsBox and TsCylinder." << G4endl;
		fPm->AbortSession(1);
	}
	// Set spatial distribution
	G4String strSpatialDistribution = "uniform";
	if (fPm->ParameterExists(GetFullParmName("SpatialDistribution"))) {
		strSpatialDistribution = fPm->GetStringParameter(GetFullParmName("SpatialDistribution"));
		strSpatialDistribution.toLower();
	}
	if (strSpatialDistribution == "uniform") {
		fSpatialDistribution = 0;
	} else if (strSpatialDistribution == "zaxis") {
		fSpatialDistribution = 1;
	} else {
		G4cerr << "Topas is exiting due to a serious error." << G4endl;
		G4cerr << GetName() << " has unsupported SpatialDistribution." << G4endl;
		G4cerr << "Supported SpatialDistribution are Uniform and Zaxis." << G4endl;
		fPm->AbortSession(1);
	}

	// Set spatial distribution
	G4String strAngularDistribution = "isotropic";
	if (fPm->ParameterExists(GetFullParmName("AngularDistribution"))) {
		strAngularDistribution = fPm->GetStringParameter(GetFullParmName("AngularDistribution"));
		strAngularDistribution.toLower();
	}
	if (strAngularDistribution == "isotropic") {
		fIsotropic = true;
	} else if (strAngularDistribution == "xminus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(-1,0,0);
	} else if (strAngularDistribution == "xplus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(+1,0,0);
	} else if (strAngularDistribution == "yminus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(0,-1,0);
	} else if (strAngularDistribution == "yplus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(0,+1,0);
	} else if (strAngularDistribution == "zminus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(0,0,-1);
	} else if (strAngularDistribution == "zplus") {
		fIsotropic = false;
		fDirection = G4ThreeVector(0,0,+1);
	} else {
		G4cerr << "Topas is exiting due to a serious error." << G4endl;
		G4cerr << GetName() << " has unsupported AngularDistribution." << G4endl;
		G4cerr << "Supported AngularDistribution are Isotropic, Xminus, Xplus, Yminus, Yplus, Zminus, and Zplus." << G4endl;
		fPm->AbortSession(1);
	}

	fVolumes = fComponent->GetAllPhysicalVolumes(fRecursivelyIncludeChildren);
	fNeedToRetrieveNavigator = true;
}

void ParticleGeneratorFanoTest::UpdateForNewRun(G4bool rebuiltSomeComponents) {
	TsVGenerator::UpdateForNewRun(rebuiltSomeComponents);
	ResolveParameters();
}


void ParticleGeneratorFanoTest::RetrieveNavigator() {

	fNeedToRetrieveNavigator = false;

	G4TransportationManager* transportationManager = G4TransportationManager::GetTransportationManager();
	fNavigator = transportationManager->GetNavigator(transportationManager->GetParallelWorld(fComponent->GetWorldName()));
}

void ParticleGeneratorFanoTest::GeneratePrimaries(G4Event* anEvent)
{
	if (CurrentSourceHasGeneratedEnough())
		return;

	if (fNeedToRetrieveNavigator)
		RetrieveNavigator();

	TsPrimaryParticle p;

	if (fIsotropic)
	{
		G4double costheta = G4RandFlat::shoot( -1., 1);
		G4double sintheta = sqrt(1. - costheta*costheta);
		G4double phi = 2.* CLHEP::pi * G4UniformRand();
		G4double sinphi = sin(phi);
		G4double cosphi = cos(phi);
		G4double px = sintheta * cosphi;
		G4double py = sintheta * sinphi;
		G4double pz = costheta;
		G4double mag = std::sqrt((px*px) + (py*py) + (pz*pz));

		p.dCos1 = px / mag;
		p.dCos2 = py / mag;
		p.dCos3 = pz / mag;
	} else {
		p.dCos1 = fDirection.x();
		p.dCos2 = fDirection.y();
		p.dCos3 = fDirection.z();
	}

	G4double testX;
	G4double testY;
	G4double testZ;
	
	G4VPhysicalVolume* foundVolume;
	G4bool foundPointInComponent = false;

	G4int counter = 0;
	while (!foundPointInComponent) {

		// Randomly sample a point inside the top-level component
		if (fSpatialDistribution) {
			// Z-axis only
			testX = 0;
			testY = 0;
		} else {
			if (fGeometryType) {
				// TsCylinder
				G4double r = fRmax*sqrt(G4UniformRand());
				G4double theta = 2.* CLHEP::pi * G4UniformRand();
				testX = r * cos(theta);
				testY = r * sin(theta);
			} else {
				// TsBox
				testX = G4RandFlat::shoot(-fHLX,fHLX);
				testY = G4RandFlat::shoot(-fHLY,fHLY);
			}
		}
		testZ = G4RandFlat::shoot(-fHLZ,fHLZ);

		// Sample random variable to reject point based on density
		G4double rejection = fRefDensity * G4UniformRand();

		// Check whether the point is inside any of the component's volumes
		foundVolume = fNavigator->LocateGlobalPointAndSetup(G4ThreeVector(testX, testY, testZ));
		
		// Protect against case where extent has extended outside of the world
		if (foundVolume) {
			// if (foundVolume->GetName()!="World") {
			if (foundVolume->GetMotherLogical()) {
				if (foundVolume->GetLogicalVolume()->GetMaterial()->GetDensity() >= rejection) {
						foundPointInComponent = foundVolume;
				}
			}
		}
		counter++;
	}
	
	p.posX = testX;
	p.posY = testY;
	p.posZ = testZ;

	SetEnergy(p);
	SetParticleType(p);

	p.weight = 1.;
	p.isNewHistory = true;

	GenerateOnePrimary(anEvent, p);
	AddPrimariesToEvent(anEvent);
}
