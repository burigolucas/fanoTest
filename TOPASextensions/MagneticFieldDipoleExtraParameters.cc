// Magnetic Field for MagneticFieldDipoleExtraParameters
//

#include "MagneticFieldDipoleExtraParameters.hh"

#include "TsParameterManager.hh"

#include "TsVGeometryComponent.hh"

#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4SimpleHeum.hh"
#include "G4SimpleRunge.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4CashKarpRKF45.hh"
#include "G4RKG3_Stepper.hh"
#include "G4ClassicalRK4.hh"
#include "G4ExplicitEuler.hh"
#include "G4ImplicitEuler.hh"
#include "G4DormandPrince745.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"


MagneticFieldDipoleExtraParameters::MagneticFieldDipoleExtraParameters(TsParameterManager* pM, TsGeometryManager* gM,
											 TsVGeometryComponent* component):
TsVMagneticField(pM, gM, component) {

	G4String stepper_name = "ClassicalRK4";
	if (fPm->ParameterExists(fComponent->GetFullParmName("FieldStepper")))
		stepper_name = fPm->GetStringParameter(fComponent->GetFullParmName("FieldStepper"));

	stepper_name.toLower();
	if (stepper_name == "expliciteuler")
		fStepper = new G4ExplicitEuler(fMagFieldEquation);
	else if (stepper_name == "impliciteuler")
		fStepper = new G4ImplicitEuler(fMagFieldEquation);
	else if (stepper_name == "simplerunge")
		fStepper = new G4SimpleRunge(fMagFieldEquation);
	else if (stepper_name == "simpleheum")
		fStepper = new G4SimpleHeum(fMagFieldEquation);
	else if (stepper_name == "helixexpliciteuler")
		fStepper = new G4HelixExplicitEuler(fMagFieldEquation);
	else if (stepper_name == "heliximpliciteuler")
		fStepper = new G4HelixImplicitEuler(fMagFieldEquation);
	else if (stepper_name == "helixsimplerunge")
		fStepper = new G4HelixSimpleRunge(fMagFieldEquation);
	else if (stepper_name == "cashkarprkf45")
		fStepper = new G4CashKarpRKF45(fMagFieldEquation);
	else if (stepper_name == "rkg3")
		fStepper = new G4RKG3_Stepper(fMagFieldEquation);
	else if (stepper_name == "dormandprince745")
		fStepper = new G4DormandPrince745(fMagFieldEquation);
	else
		fStepper = new G4ClassicalRK4(fMagFieldEquation);

	// delete chord finder created in base class
	if(fChordFinder) delete fChordFinder;

	G4double StepMin = 1.0e-2 * mm;
	if (fPm->ParameterExists(fComponent->GetFullParmName("FieldStepMinimum")))
		StepMin = fPm->GetDoubleParameter(fComponent->GetFullParmName("FieldStepMinimum"), "Length");

	G4double dChord = 1.0e-1 * mm;;
	if (fPm->ParameterExists(fComponent->GetFullParmName("FieldDeltaChord")))
		dChord = fPm->GetDoubleParameter(fComponent->GetFullParmName("FieldDeltaChord"), "Length");

	fChordFinder = new G4ChordFinder(this, StepMin, fStepper);
	fChordFinder->SetDeltaChord(dChord);

	fFieldManager->SetChordFinder(fChordFinder);

	G4double valueD1step = 0.01 * millimeter;
	if (fPm->ParameterExists(fComponent->GetFullParmName("DeltaOneStep")))
		valueD1step = fPm->GetDoubleParameter(fComponent->GetFullParmName("DeltaOneStep"), "Length");
	fFieldManager->SetDeltaOneStep(valueD1step); 

	G4double valueDintersection = 0.001 * millimeter;
	if (fPm->ParameterExists(fComponent->GetFullParmName("DeltaIntersection")))
		valueDintersection = fPm->GetDoubleParameter(fComponent->GetFullParmName("DeltaIntersection"), "Length");
	fFieldManager->SetDeltaIntersection(valueDintersection); 

	G4double newEpsMin = 5.0e-5;
	if (fPm->ParameterExists(fComponent->GetFullParmName("MinimumEpsilonStep")))
		newEpsMin = fPm->GetUnitlessParameter(fComponent->GetFullParmName("MinimumEpsilonStep"));
	fFieldManager->SetMinimumEpsilonStep(newEpsMin); 

	G4double newEpsMax = 1.0e-3;
	if (fPm->ParameterExists(fComponent->GetFullParmName("MaximumEpsilonStep")))
		newEpsMax = fPm->GetUnitlessParameter(fComponent->GetFullParmName("MaximumEpsilonStep"));
	fFieldManager->SetMaximumEpsilonStep(newEpsMax);

	ResolveParameters();
}

MagneticFieldDipoleExtraParameters::~MagneticFieldDipoleExtraParameters() {;}


void MagneticFieldDipoleExtraParameters::ResolveParameters() {
	G4ThreeVector direction;
	direction.setX(fPm->GetUnitlessParameter(fComponent->GetFullParmName("MagneticFieldDirectionX")));
	direction.setY(fPm->GetUnitlessParameter(fComponent->GetFullParmName("MagneticFieldDirectionY")));
	direction.setZ(fPm->GetUnitlessParameter(fComponent->GetFullParmName("MagneticFieldDirectionZ")));
	direction.unit();

	G4double strength = fPm->GetDoubleParameter(fComponent->GetFullParmName("MagneticFieldStrength"), "Magnetic flux density");
	fFieldValue.setX(strength * fComponent->GetEnvelopePhysicalVolume()->GetRotation()->colX() * direction);
	fFieldValue.setY(strength * fComponent->GetEnvelopePhysicalVolume()->GetRotation()->colY() * direction);
	fFieldValue.setZ(strength * fComponent->GetEnvelopePhysicalVolume()->GetRotation()->colZ() * direction);
}


void MagneticFieldDipoleExtraParameters::GetFieldValue(const G4double Point[3], G4double* Field) const{
	// This point can be useful if your field depends on location within the component
	const G4ThreeVector localPoint = fNavigator->GetGlobalToLocalTransform().TransformPoint(G4ThreeVector(Point[0],Point[1],Point[2]) );

	Field[0] = fFieldValue.x();
	Field[1] = fFieldValue.y();
	Field[2] = fFieldValue.z();
}
