# -*- coding: utf8 -*-
"""
=============VIRTUAL'SUN=============
Générateur de soleil pour Virtuelium

--> Classe :
    Soleil : Un soleil en kit

--> Conventions :
    Dans une scène Virutelium, le plan horizontal correspond au plan formé par les axes (Ox) et (Oy).
    L'axe (Oz) est l'axe vertical orienté vers le haut. Le jour, le soleil éclaire la région z>0.
    Les directions x et y correspondent respectivement à l'est et au nord de la scène.

--> Exemple d'utilisation :
    monSoleil = Soleil() # Crée un nouveau soleil !
    monSoleil.placer(48,2,datetime.now()) # Place le soleil dans le ciel tel qu'il est maintenant en région parisienne
    
    print "Il fait jour ?", monSoleil.jour
    print "Le soleil est en position :", monSoleil.azimut, monSoleil.altitude
    
    monSpectre = monSoleil.genererSpectre()
    print monSoleil.genererXML(10, comment = (str(datetime.utcnow())), monSpectre)

2011 / Matthieu Ancellin, avec la contribution de Hamza El Yachioui, Nicolas Dumont et Yannis Bekel
"""

from math import pi, sin, cos, atan, asin, acos, sqrt, exp
from datetime import datetime, timedelta
from XMLSpectrum import *
from os.path import join

class Soleil () :
    """Un soleil intégrable dans Virtuelium

    Attributs :
        azimut - float : angle en degrés sur le plan horizontal par rapport au Nord dans le sens horaire
        altitude - float : altitude du soleil au dessus de l'horizon en degrés
    """

    def __init__ (self) :
        self.altitude = None
        self.azimut = None

    @property
    def jour (self) :
        """Indicateur booléen :
            True : il fait jour
            False : il fait nuit"""
        return self.altitude > -0.25
    
    def _jourJulien (self, annee, mois, jour, heure, minute, seconde, microsecond) :
        """Calcul intermédiaire pour la position astronomique du soleil :
        nombre de jours depuis un jour de référence.

        Arguments :
            annee, mois, jour, heure, minute, seconde, microsecond - float(s) : instant

        Return :
            float : jour julien (nombre de jour depuis le jour de référence)"""
        jour = jour + heure / 24.0 + minute / 1440.0 + seconde / 86400.0 + microsecond / 86400000000.0

        if mois == 1 or mois == 2 :
            annee = annee - 1.0
            mois = mois + 12.0

        a = int(annee/100)
        b = 2 - a + int(a/4) #Correction pour calendrier grégorien (15 oct. 1582)
        return int(365.25*(annee+4716.0))+int(30.6001*(mois+1.0))+jour+b-1524.5

    def placer (self, latitude, longitude, instant, correctionGMT = 2) :
        """Calculs astronomiques pour mettre à jour la position du soleil

        Arguments :
            latitude - float : latitude du lieu d'observation dans l'intervalle [-180°, 180°]
            longitude - float : longitude du lieu d'observation dans l'intervalle [-90°, 90°]
            instant - datetime : date (calendrier grégorien) et heure locale
            correctionGMT - float : difference avec le fuseau horaire de Greenwitch
                                    en France, correctionGMT = 2 en été, 1 en hiver
        """

        jourJ = self._jourJulien(instant.year, instant.month, instant.day, instant.hour, instant.minute, instant.second, instant.microsecond)-2451545.0
        
        g = 357.529+0.98560028*jourJ
        q = 280.459+0.98564736*jourJ
        l = q+1.915*sin(g*pi/180.)+0.020*sin(2*g*pi/180.)
        e = 23.439-0.00000036*jourJ

        ascension_droite = atan(cos(e*pi/180.)*sin(l*pi/180.)/cos(l*pi/180.))* (180./pi) /15.0

        if cos(l*pi/180.)<0:
            ascension_droite = 12.0+ascension_droite
        if cos(l*pi/180.)>0 and sin(l*pi/180.)<0 :
            ascension_droite = ascension_droite+24.0

        declinaison = asin(sin(e*pi/180.)*sin(l*pi/180.)) * 180./pi

        nb_siecle = jourJ/36525.0
        heure_siderale1 = (24110.54841+(8640184.812866*nb_siecle)+(0.093104*(nb_siecle**2))-(0.0000062*(nb_siecle**3)))/3600.0
        heure_siderale2 = ((heure_siderale1/24.0)-int(heure_siderale1/24.0))*24.0

        angleH = 360.*heure_siderale2/23.9344
        angleT = (instant.hour-correctionGMT-12.0+instant.minute/60.0+instant.second/3600.0)*360/23.9344

        angle = angleT+angleH
        angle_horaire = angle - ascension_droite*15.0+longitude #15° = 360° / 24h
        
        self.altitude = asin(sin(declinaison*pi/180.)*sin(latitude*pi/180.)-cos(declinaison*pi/180.)*cos(latitude*pi/180.)*cos(angle_horaire*pi/180.)) *180./pi
        self.azimut = acos((sin(declinaison*pi/180.)-sin(latitude*pi/180.)*sin(self.altitude*pi/180.))/(cos(latitude*pi/180.)*cos(self.altitude*pi/180.))) *180./pi
        sinazimut = (cos(declinaison*pi/180.)*sin(angle_horaire*pi/180.))/cos(self.altitude*pi/180.)
        if sinazimut<0 :
            self.azimut = 360-self.azimut;

    def genererSpectre(self, dossierData = join("data", "colorimetry", "illuminants"), indice = "") :
        """Crée le spectre solaire artificiel associé à la position du soleil
        dans le ciel

        /!\ Le soleil doit avoir été placé

        Arguments :
            dossierData - string : (facultatif) dossier où sera enrigistré le fichier XML
            indice - string : (facultatif) indice inséré dans le nom du fichier spectre crée

        Retour :
            string : chemin du fichier spectre créé"""

        if self.altitude == None or self.azimut == None :
            raise Exception ("Le soleil doit avoir été placé !")

        data = XMLtoArray("python/solar/SpectreReference.xml")
        
        R = 6371000 #Rayon de la terre
        r = 8000 #Epaisseur de la couche atmosphérique
        
        z = - R * sin(self.altitude*pi/180.) + sqrt((R+r)*(R+r) - R*R*cos(self.altitude*pi/180.)*cos(self.altitude*pi/180.))
        #Couche d'atmosphere traversee

##        import matplotlib.pyplot as plt
##        import numpy as np
##        plt.plot(np.asarray(data)[:,0], np.asarray(data)[:,1])

        k = 32/3 * pi*pi*pi * 3e-4 * 3e-4 / (2.6e25)
        data = map(lambda (l,p) : (l, p* exp(-k /(l*1e-9)**4 *z)), data)

##        plt.plot(np.asarray(data)[:,0], np.asarray(data)[:,1])
##        plt.xlabel("Lambda (nm)")
##        plt.ylabel("Puissance spectrale en unites arbitraires")
##        plt.show()

        data = normaliser(data)
        
        filename = join(dossierData, "Soleil" + indice+ ".xml")
        ArraytoXML(data, filename)

        return filename        

    def genererXML (self, distance = 10., comment = "", spectre="data/colorimetry/illuminants/D65.xml") :
        """Génere le code XML correspondant au soleil dans un fichier scène Virutuelium

        /!\ Le soleil doit avoir été placé

        Arguments :
            distance - float : (facultatif) distance de la sphère du soleil par rapport à l'origine de la scène
            comment - string : (facultatif) texte inséré dans le commentaire en tête du bloc XML
            spectre - string : (facultatif) chemin du fichier spectre XML utilisé

        Retour :
            string : code XML"""

        if self.altitude == None or self.azimut == None :
            raise Exception ("Le soleil doit avoir été placé !")
      
        x = cos(self.altitude * pi/180.) * distance * sin(self.azimut * pi/180)
        y = cos(self.altitude * pi/180.) * distance * cos(self.azimut * pi/180)
        z = sin(self.altitude * pi/180.) * distance
        rayon = distance * 16 / 360 * pi / 180 #Diametre apparent : 32'

        return """<!-- Soleil {4}-->
<source name="Soleil">
    <geometry type="Sphere" radius="{0}" centerx="{1}" centery="{2}" centerz="{3}" />
        <source type="Point" spectrum="{6}" power="{5}"
            originx="{1}" originy="{2}" originz="{3}" radius="{0}" />
        </source>""".format(rayon, x, y, z, comment, 4*3.14*200*distance*distance, spectre)
