# -*- coding: utf8 -*-
"""
=============VIRTUAL'SUN=============
Générateur de camera pour Virtuelium

--> Classe :
    CameraGeometry : Balise <geometry> de la Camera

--> Conventions :

--> Exemple d'utilisation :

2013 / Programmix
"""

from math import pi, sin, cos, atan, asin, acos, sqrt, exp
from os.path import join

class Point( object ):
    """Un point de l'espace

    Attributs :
        x - float : Dimension 1
        y - float : Dimension 2
        z - float : Dimension 3
    """
    def __init__( self, x = 0, y = 0, z = 0):
        self.x, self.y, self.z = x, y, z

    @property
    def Normalize(self) :
        n = sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
        self.x = self.x / n
        self.y = self.y / n
        self.z = self.z / n
    
    def Dot(self, second) :
        result = 0
        result = result + self.x * second.x
        result = result + self.y * second.y
        result = result + self.z * second.z
        return result        
    
    def Show(self) :
        print """({0}, {1}, {2})""".format(self.x, self.y, self.z)
        
class CameraGeometryRotation () :
    """Une geometrie de camera en rotation integrable dans Virtuelium

    Attributs :
        cam - Point : Camera à transformer
        matrix_1 - Point : Ligne 1 de la matrice de rotation
        matrix_2 - Point : Ligne 2 de la matrice de rotation
        matrix_3 - Point : Ligne 3 de la matrice de rotation
    """

    def __init__ (self, cam, u, theta) :
        self.cam = cam
        u.Normalize
        
        # Calcule la matrice de rotation
        c = cos(theta)
        s = sin(theta)
        ic = 1 - c
        
        self.matrix_1 = Point((u.x * u.x) + ((1 - u.x * u.x) * c),
                              (u.x * u.y * ic) - (u.z * s),
                              (u.x * u.z * ic) + (u.y * s)) 
        self.matrix_2 = Point((u.x * u.y * ic) + (u.z * s),
                              (u.y * u.y) + ((1 - u.y * u.y) * c),
                              (u.y * u.z * ic) - (u.x * s)) 
        self.matrix_3 = Point((u.x * u.z * ic) - (u.y * s),
                              (u.y * u.z * ic) + (u.x * s),
                              (u.z * u.z) + ((1 - u.z * u.z) * c))
        
    @property
    def Rotate(self) :
        """Transforme la camera

        Arguments :
            
        Retour :
            La position de la camera
        """

        rot = Point()
        rot.x = self.cam.Dot(self.matrix_1)
        rot.y = self.cam.Dot(self.matrix_2)
        rot.z = self.cam.Dot(self.matrix_3)
        return rot
        
    def genererXML (self, origin, lookat, up, fov, type = "Perspective", comment = "") :
        """Génere le code XML correspondant a la geometrie de camera dans un fichier scène Virutuelium

        Arguments :

        Retour :
            string : code XML
        """
        
        return """<!-- Camera {0} -->
<geometry type="{1}"
       fov="{2}"
       originx="{3}"  originy="{4}" originz="{5}"
       lookatx="{6}"  lookaty="{7}" lookatz="{8}"
       upx="{9}"      upz="{10}"     upz="{11}" />""".format(comment, type, fov, 
                                                             origin.x, origin.y, origin.z, 
                                                             lookat.x, lookat.y, lookat.z, 
                                                             up.x, up.y, up.z)

