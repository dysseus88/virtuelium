import sys
sys.path.append("python/solar")

from VirtualSun import *
from Animation import *

maintenant = datetime(2011,10,07,14,00,00)
# (ou)
#maintenant = datetime.now()

pasDeTemps = timedelta(minutes = 60)
monSoleil = Soleil()

N = 3

xmls = []
for i in range(N) :
    monSoleil.placer(43.53, 5.45, maintenant, 2)
    xmls.append(monSoleil.genererXML(50.0, str(maintenant) , spectre = monSoleil.genererSpectre(indice = str(i))))
    maintenant = maintenant + pasDeTemps

genererScenes("./projects/House/House.xml", xmls)
    
calculerScenes("./projects/House/House.xml", N)


