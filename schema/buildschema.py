'''
buildschema.py - Compiles individual schema files into a single complete 
                 JSON schema and creates a C++ header file containing 
                 the escaped schemas.
'''
import re 
import os
import json
import sys 


# Definitions
currd = os.path.dirname(os.path.abspath(__file__))
cdir = "combined/"
schemah = '../include/schema.h'

# All files
files = [
	("connectivities/connectivities.json", "Connectivities"),
	("connectivities/P2SA.connectivity.json", "P2SAConnectivity"),
	("ensembles/ensembles.json", "Ensembles"),
	("ensembles/nvt.ensemble.json", "NVTEnsemble"),
	("ensembles/gibbsnvt.ensemble.json", "GibbsNVTEnsemble"),
	("moves/flipspin.move.json", "FlipSpinMove"),
	("moves/identitychange.move.json", "IdentityChangeMove"),
	("moves/moves.json", "Moves"),
	("moves/particleswap.move.json", "ParticleSwapMove"),
	("moves/speciesswap.move.json", "SpeciesSwapMove"),
	("moves/sphereunitvector.move.json", "SphereUnitVectorMove"),
	("moves/translate.move.json", "TranslateMove"),
	("observers/csv.observer.json", "CSVObserver"),
	("observers/observer.json", "Observer"),
	("particles/director.particle.json", "Director"),
	("particles/selector.particle.json", "Selector")
	]

# Files to generate combined 
cfiles = [
	("connectivities/connectivities.json", cdir + "connectivities.json"),
	("ensembles/ensembles.json", cdir + "ensembles.json"),
	("moves/moves.json", cdir + "moves.json")
	]

def processfile(filename):
	fpath = os.path.dirname(filename)
	with open(filename, 'r') as f:
		text = f.read()
		for match in re.findall('"@file[(](.*)[)]"', text):
			text = text.replace(
				'"@file(' + match + ')"', 
				processfile(fpath + "/" + match)
			)
		return text

def gencombined(inoutfile):
	jtext = processfile(inoutfile[0])
	jobj = json.loads(jtext)
	with open(inoutfile[1], 'w') as outfile:
		json.dump(jobj, outfile, indent=4, separators=(',', ': '))

def genheader(inputfd):
	fout = os.path.join(currd, schemah)
	fin = fout 
	if not os.path.isfile(fin):
		fin = os.path.join(currd, "headertemplate.h")

	jobj = json.loads(processfile(inputfd[0]))
	decl = '\t\tstatic std::string ' + inputfd[1] + ';\n'
	defn = 'std::string SAPHRON::JsonSchema::' + inputfd[1] + ' = '

	lines = []
	with open(fin, "r") as f:
		lines = f.readlines()
		i = next(j for j, s in enumerate(lines) if 'INSERT_DEC_HERE' in s)
		lines.insert(i+1, decl)
		i = next(j for j, s in enumerate(lines) if 'INSERT_DEF_HERE' in s)
		lines.insert(i+1, defn + '"' + json.dumps(jobj).replace('"', '\\"') + '";\n')

	with open(fout, "w") as f:
		f.writelines(lines)

if len(sys.argv) == 2 and sys.argv[1] == "clean":
	fdir = currd + "/" + cdir
	for f in os.listdir(fdir):
		fpath = os.path.join(fdir, f)
		os.remove(fpath)
else:
	for f in cfiles:
		gencombined(f)

	for f in files:
		genheader(f)