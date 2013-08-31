import sys
sys.path.append("python/camera")

from CameraRotation import *
from Animation import *

print("TODO: Axe ne passant par l'origine")
print("TODO: Meilleure vitesse angulaire")
print("TODO: Commentaire")

rotCenter = Point()
camOrigin = Point(0.5, 0, 0.1)
up = Point(0, 0, 1)
fov = 13

axis = Point(0, 0, 1)
pasDegree = 10
maxDegree = 90
N = 0
curAngleDegree = 0
xmls = [] 
while curAngleDegree <= maxDegree :
    curAngleRad = curAngleDegree * pi / 180    
    maCamera = CameraGeometryRotation(camOrigin, axis, curAngleRad)
    xmls.append(maCamera.genererXML(maCamera.Rotate, rotCenter, up, fov, comment = """{0} degree""".format(curAngleDegree)))
    curAngleDegree = curAngleDegree + pasDegree
    N = N + 1
    
genererScenes("./projects/DynamicCamera/DynamicCamera.xml", xmls)
    
calculerScenes("./projects/DynamicCamera/DynamicCamera.xml", N, mt=4)


