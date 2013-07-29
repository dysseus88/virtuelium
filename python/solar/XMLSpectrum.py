# -*- coding: utf8 -*-
"""
Outils de gestion des spectres et des fichiers spectres XML pour Virtuelium

--> Fonctions :
    toXMLSpectrum : converti un fichier données brut en fichier spectre XML
    XMLtoArray : lecture d'un fichier XML pour être exploité par Python
    ArraytoXML : opération inverse à la précedente

--> Exemples d'utilisation :
    toXMLSpectrum("idalion794.txt", "data/illuminants/idalion794.xml")
    donnees = XMLtoArray("data/illuminants/idalion794.xml")
    donnees = actionQuelconque(donnees)
    ArraytoXML(donnees, "data/illuminants/idalion794.xml")

2011 / Matthieu Ancellin
"""

def toXMLSpectrum (entree, sortie, intervalle = (380, 780)) :
    """Converti un fichier spectre tel qu'issu du spectromètre en fichier XML
    utilisable par Virtuelium

    Arguments :
        entree - string : chemin du fichier original
        sortie - string : chemin du nouveau fichier
        intervalle - tuple of int : (facultatif) seules les valeurs dans cet intervalle
                                                de longueurs d'ondes sont prises en comptes
    """
    outputFile = open(sortie,'w')
    inputFile = open(entree, 'r')
    outputFile.write("<illuminant>  ")
    
    for line in inputFile :
        if line[0] in '0123456789' :
            line = line.replace(',', '.')
            data = line.split()
            if float(data[0]) > intervalle[0] and float(data[0]) < intervalle[1] :
                outputFile.write('<entry wavelength="' + str(data[0]) + '" value="' + str(data[1]) + '" /> ')
    outputFile.write("</illuminant>")
    
    inputFile.close()
    outputFile.close()

def XMLtoArray (entree) :
    """Détricote un fichier spectre XML et le transforme en tableau de valeurs pour Python

    Arguments :
        entree - string : chemin du fichier spectre XML

    Retour :
        list of tuple of float : donnees spectrales sous la forme d'une liste de doublets (lambda, P(lambda))
    """
    inputFile = open(entree, 'r')
    entries = inputFile.read().split('<entry')
    data = []
    for entry in entries :
        values = entry.split('"')
        values = filter(lambda v : (v[0] in '0123456789.'), values)
        values = map(float, values)
        if len(values) == 2 :
            data.append(tuple(values))
    inputFile.close()

    return data
    

def ArraytoXML (data, sortie, intervalle = (380, 780), comment = "") :
    """Transforme une tableau de valeurs spectrales en fichier spectre XML utilisable par Virtuelium

    Arguments :
        data - list of tuple of float : données spectrale sous forme tabulaire
        sortie - string : chemin du fichier de destination
        intervalle - tuple of int : (facultatif) seules les valeurs dans cet intervalle
                                                de longueurs d'ondes sont prises en comptes
        comment - string : (facultatif) commentaire inséré en tête du fichier XML"""
    outputFile = open(sortie,'w')
    outputFile.write("<!-- " + comment + " -->")
    outputFile.write("<illuminant>  ")
    for d in data :
        if float(d[0]) > intervalle[0] and float(d[0]) < intervalle[1] :
            outputFile.write('<entry wavelength="' + str(d[0]) + '" value="' + str(d[1]) + '" />  ')
    outputFile.write("</illuminant>")

    outputFile.close()

def normaliser (data) :
    """Normalise le spectre d'une source : ajuste à 100 la valeur associée à 560nm

    Argument :
        data - list of tuple of float : données spectrales

    Retour :
        list of tuple of float : données spectrales normalisées
    """
    l = data[0][0]
    #Recherche de la premiere longueur d'onde supérieure ou égale à 560
    #On supposera qu'elle est bien trés voisine de 560 nm
    i = 0
    while l < 560.0 :
        i += 1
        l = data[i][0]

    coeff = 100.0/data[i][1]

    #Normalisation de toutes les valeurs
    data = map(lambda (l,x) : (l,x*coeff), data)

    return data
