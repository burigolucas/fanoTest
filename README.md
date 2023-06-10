# Fano Cavity Tests

## Background

Fano test is used to check the consistency of the Monte Carlo transport algorithm of charged particles. It is based on the Fano theorem (Fano, 1954):
>in a medium of given composition exposed to a uniform flux of primary radiation (such as x-rays or neutrons), the flux of secondary radiation is also uniform and independent of the density of the medium as well as of the density variations from point to point

For the Fano test, the primary radiation (photons) should be uniform. Then, the number of secondary particles created in a given volume will scale with the mass density because the probability of an interaction in the volume scales with the mass. In this way, the density of the secondary particles (electrons) will not be uniform across volumes of different mass density. However, the density of secondary particles per unit mass is uniform assuming charge particle equilibrium.

Then, to apply the Fano test for a primary beam of photons, we need to either:

- generate an uniform source of photons
- generate an uniform source of electrons per unit mass

When generating the primary source (photons), either a regeneration technique (Kawrakow 2000) or a re-entrance technique (Yi et al 2006) has to be used. An alternative approach was proposed by Sempau and Andreo (2006) by directly simulating the secondary radiation field.

The Fano test should be applied prior to the simulation of the response of ionization chambers to verify the consistency of the Monte Carlo transport code. According to de Pooter at al (2015):
 > due to the expected sensitivity of the Fano test to the geometry, energy, and transport parameters, we recommend carefully validating each simulation prior to use with and without a B-field, rather than using a pre-benchmarked code without further consideration. This can be achieved by simulating the beam transport in the geometry where all materials are replaced by water of varying densities, which should allow the determination of the code self-consistency for specific applications.

## Special Conditions in Magnetic Fields

The classical Fano test is not applicable in the presence of magnetic fields (Bouchard and Bielajew 2015). Two special Fano test cases have been proposed in the presence of magnetic fields (Bouchard et al 2015):

### Condition 1:

- generate an uniform and isotropic source of photons
- generate an uniform and isotropic source of electrons per unit mass

### Condition 2:

- generate an uniform source of photons with the B-field at constant direction and strength proportional to the mass
- generate an uniform source of electrons per unit mass with the B-field at constant direction and strength proportional to the mass

## TOPAS extensions

Please, follow the instructions from TOPAS guide on how to compile and link custom TOPAS extensions.

### Fano Test Particle Generator

`ParticleGeneratorFanoTest` implements a particle source extension to be used for Fano tests. It should be used for the generation of the secondary charged particles directly (i.e., electrons) based on the approach from Sempau and Andreo (2006). The current implementation only supports the generation of an isotropic source of particles per unit mass either uniformly distributed in a component, or only along the Z-axis (when using the reciprocity theorem). The source can only be assigned to a geometry component of type `TsBox` or `TsCylinder`. The particles will be generated inside the component and any sub-component. The material of the component should correspond to the material with hightest density in the sampling region.

In order to sample an uniform and isotropic distribution of particles per unit mass, the rejection sampling method is applied. First, a position $(x,y,z)$ is sampled from uniform distributions. Then, the ratio of the local density to the reference density is used to establish the rejection threshold.

**NOTE** The current implementation does not support translations and rotations of the source component.

**TODO** Add variance reduction technique with generation of multiple electrons in the cavity.

Example of use:

```
s:So/MyFanoSource/Type = "FanoTest"
s:So/MyFanoSource/Component = "Component1"
s:So/MyFanoSource/SpatialDistribution = "Uniform"
s:So/MyFanoSource/BeamParticle = "e-"
d:So/MyFanoSource/BeamEnergy = 1. MeV
u:So/MyFanoSource/BeamEnergySpread = 0.
i:So/MyFanoSource/NumberOfHistoriesInRun = 10000000
```

The spatial distribution can be either `Uniform` or `Zaxis`.

### Density correction toggle

`DensityCorrectionToggle` allows to disable (effectively setting to zero) the density correction in the Bethe-Bloch equations. This is necessary to perform the Fano test, since the density effect correction is enabled by default in Geant4. To disable to density effect for a given list of materials used in the Fano test, add the follwoing TOPAS parameter to the TOPAS parameter file:

```
sv:Fano/DisableDensityEffectCorrection = 2 "Water_low" "Water_high"
```

In the example above, the density effect correction for the energy loss is disabled for the materials `"Water_low"` and `"Water_high"`.

The implementation makes use of a user hook at the begin of the Geant4 run to modify the density correction parameter.

### Dipole magnetic field with extra TOPAS parameters for transport equations

`MagneticFieldDipoleExtraParameters` implements a simple uniform dipole magnetic field and exposes several Geant4 parameters for the charged-particle transport in magnetic field as TOPAS parameters. This allows to investigate the effect of the transport parameters on the Fano test by controlling the parameters directly from the TOPAS parameter file. 

## References

- Fano U 1954 Note on the Bragg-Gray cavity principle for measuring energy dissipation *Radiat. Res.* **1**: 237–40
- Bouchard H and Bielajew A 2015 Lorentz force correction to the Boltzmann radiation transport
equation and its implications for Monte Carlo algorithms *Phys. Med. Biol.* **60**: 4963–71
- Bouchard H, de Pooter J, Bielajew A and Duane S 2015 Reference dosimetry in the presence of magnetic fields: conditions to validate Monte Carlo simulations *Phys. Med. Biol.* **60**: 6639–54
- Kawrakow I 2000 Accurate condensed history Monte Carlo simulation of electron transport. II.
Application to ion chamber response simulations *Med. Phys.* **27**: 499–513
- O'Brien DJ, Roberts DA, Ibbott GS, Sawakuchi GO. Reference dosimetry in magnetic fields: formalism and ionization chamber correction factors. Med Phys. 2016 43: 4915. DOI: 10.1118/1.4959785
- Sempau J and Andreo P 2006 Configuration of the electron transport algorithm of PENELOPE to
simulate ion chambers *Phys. Med. Biol.* **51**: 3533–48
- Simiele E and DeWerd L. On the accuracy and efficiency of condensed history transport in magnetic fields in GEANT4. 2018 Phys. Med. Biol. 63: 235012. DOI: https://doi.org/10.1088/1361-6560/aaedc9
- de Pooter, JA et al 2015  Application of an adapted Fano cavity test for Monte Carlo simulations in the presence of B-fields *Phys. Med. Biol.* **60**: 9313–28
- Yi C-Y, Hah S-H and Yeom M S 2006 Monte Carlo calculation of the ionization chamber response to 60Co beam using PENELOPE *Med. Phys.* **33**: 1213–21