//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// Hadrontherapy advanced example for Geant4
// See more at: https://twiki.cern.ch/twiki/bin/view/Geant4/AdvancedExamplesHadrontherapy

#include "ScintillatorSD.hh"
#include "ScintillatorHit.hh"

//#include "HadrontherapyMatrix.hh"
//#include "HadrontherapyLet.hh"
//#include "HadrontherapyRunAction.hh"
//#include "HadrontherapySteppingAction.hh"

#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "G4SteppingManager.hh"
#include "G4TrackVector.hh"
#include "G4ios.hh"
#include "G4SteppingManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4TrackVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4UserEventAction.hh"
#include "G4TransportationManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"


/////////////////////////////////////////////////////////////////////////////
ScintillatorSD::ScintillatorSD(G4String name):
G4VSensitiveDetector(name)
{
    G4String HCname;
    collectionName.insert(HCname="ScintillatorHitsCollection");
    HitsCollection = NULL;
    sensitiveDetectorName = name;
    
}

/////////////////////////////////////////////////////////////////////////////
ScintillatorSD::~ScintillatorSD()
{}

/////////////////////////////////////////////////////////////////////////////
void ScintillatorSD::Initialize(G4HCofThisEvent*)
{
    
    HitsCollection = new ScintillatorHitsCollection(sensitiveDetectorName,
                                                             collectionName[0]);
}

/////////////////////////////////////////////////////////////////////////////
G4bool ScintillatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* )
{
   // G4cout << "Physical Volume: " << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName() << G4endl;

    if (aStep -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName() != "Layer") return false;
    
    
    // Get kinetic energy
    G4Track * theTrack = aStep  ->  GetTrack();
   
    G4ThreeVector stepDelta = aStep->GetDeltaPosition();
    G4double direction = stepDelta.getZ();

    G4ParticleDefinition *particleDef = theTrack -> GetDefinition();
    //Get particle name
    G4String particleName =  particleDef -> GetParticleName();
    
    // Get particle PDG code
    G4int pdg = particleDef ->GetPDGEncoding();
    
    // Get unique track_id (in an event)
    G4int trackID = theTrack -> GetTrackID();
    
    G4double energyDeposit = aStep -> GetTotalEnergyDeposit();
    
    G4double DX = aStep -> GetStepLength();
    //G4int Z = particleDef-> GetAtomicNumber();
    //G4int A = particleDef-> GetAtomicMass();
    G4StepPoint* PreStep = aStep->GetPreStepPoint();
    
    // Position
    G4ThreeVector pos = PreStep->GetPosition();
    G4double z = pos.getZ();

    G4ThreeVector vertex = theTrack->GetVertexPosition();
    G4double origin = vertex.getZ();
    G4double tracklength = z - origin;

    // Read voxel indexes: i is the x index, k is the z index
    const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int k  = touchable->GetReplicaNumber(0);
    //G4int i  = touchable->GetReplicaNumber(2);
    //G4int j  = touchable->GetReplicaNumber(1);
    
    G4TouchableHandle touchPreStep = PreStep->GetTouchableHandle();
    G4VPhysicalVolume* volumePre = touchPreStep->GetVolume();
    G4String namePre = volumePre->GetName();
    
    
    if( direction>0 && DX>0 && trackID==1 ) {
    		    
                  
        // Get the pre-step kinetic energy
        G4double eKinPre = aStep -> GetPreStepPoint() -> GetKineticEnergy();
        // Get the post-step kinetic energy
        G4double eKinPost = aStep -> GetPostStepPoint() -> GetKineticEnergy();
        // Get the step average kinetic energy
        G4double eKinMean = (eKinPre + eKinPost) * 0.5;
        


        ScintillatorHit* detectorHit = new ScintillatorHit();

        // Make this kinetic energy and position
        detectorHit -> SetKinEnAndPosition(k, tracklength, energyDeposit, eKinMean);
        HitsCollection -> insert(detectorHit);

//	G4cout << "Replica: "       << k << G4endl;
//	G4cout << "tracklength: "   << tracklength << G4endl;
//	G4cout << "energyDeposit: " << energyDeposit << G4endl;
//	G4cout << "eKinMean: "      << eKinMean << G4endl;


    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////
void ScintillatorSD::EndOfEvent(G4HCofThisEvent* HCE)
{
    
    static G4int HCID = -1;
    if(HCID < 0)
    {
        HCID = GetCollectionID(0);
    }
    
    HCE -> AddHitsCollection(HCID,HitsCollection);
}
