# -*- coding: utf8 -*-
"""
Module Animations pour Virtuelium

Crée plusieurs fichiers scénes à partir d'un fichier modèle. Lance le calcul de ces fichiers.

--> Fonctions :
    _editerNom : gère les noms des fichiers créés
    genererScenes : crée les nouveaux fichiers scènes
    parametresAnimation : crée une liste de paramètres pour genererScenes
    calculerScenes : lance le calcul de tous les fichiers scènes par Virtuelium

--> Conventions fichier modèle :
    Deux clés seront remplacées dans le fichier scène modèle :
    la première est par défaut <!--Lambda--> (attention à la casse !) 
	elle sera remplacée par les paramètres passés en argument ;
        c'est le paramètre qui varie au cours de l'animation.
    la seconde est par défaut <!--Nom-->
	elle sera remplacée par l'indice courant dans la liste des paramètres ;
        elle permet de nommer les fichiers images associés aux scènes créées.

--> Exemple d'utilisation :
    translation = parametresAnimation(5, 0.5, 25)
    genererScenes("exemple.xml", translation, sDossier = "exempleTranslation")
    calculerScene("exemple.xml", len(translation))

2011 / Matthieu Ancellin
"""
from os import system, mkdir
from os.path import split, splitext, join, exists, dirname

def _editerNom (original, sousDossier, indice, extension = None) :
    """Ecrit un nouveau nom de fichier à partir de l'original
	le nouveau fichier sera place dans sousDossier

    Arguments :
        original - str : chemin du fichier original
        sousDossier - str : nom du sous-dossier
        indice - str : indice du fichier
        extension - str : (facultatif) extension du nouveau fichier, 
			si non donnée, l'extension originale est conservée

    Exemple :
	>>> _editerNom("/home/toto/exemple.xml", "Dossier", "4", ".png")
	"/home/toto/Dossier/exemple4.png"
    """
    dossier, fichier = split(original)
    nom, ext = splitext(fichier)
    nNom = join(dossier, sousDossier)
    if extension == None :
        nNom = join(nNom, nom + indice + ext)
    else :
        nNom = join(nNom, nom + indice + extension)
    return nNom

def genererScenes (original, parametres , clef1 = "<!--Lambda-->", clef2 = "<!--Nom-->", sDossier = "scenesAnimation") :
    """Genere plusieurs fichiers scènes numerotés
	en remplacant les clefs passées en arguments
	par le paramètre variable.

    Arguments :
        original - str : chemin du fichier modèle
        parametres - list : liste des valeurs du paramètre variable
        clef1 - str : (facultatif) clef remplacée par le paramètre
        clef2 - str : (facultatif) clef remplacée par l'indice du fichier
        sDossier - str : (facultatif) sous-dossier dans lequel 
					les fichiers scénes seront enregistrés
    """
    if not original[len(original) - 4:] == ".xml" :
        raise Exception ("Fichier non valide : l'extension n'est pas XML")
    
    print "Ouverture du fichier " + original
    fichier = open(original)
    originalStr = fichier.read()
    fichier.close()

    #Création du sous-dossier s'il n'existe pas encore
    if not exists(join(dirname(original), sDossier)) :
        mkdir(join(dirname(original), sDossier))

    for i, p in enumerate(parametres) :
        newStr = originalStr.replace(clef1, str(p))
        newStr = newStr.replace(clef2, _editerNom(original, "images", str(i), ".png") )
        newName = _editerNom(original, sDossier, str(i))

        print "Enregistrement de " + newName
        fichier = open(newName, 'w')
        fichier.write(newStr)
        fichier.close()

def parametresAnimation (t,  dlambda, ips = 25) :
    """Crée une liste de paramètres pour une animation.

    Arguments :
        t - float : durée de l'animation en secondes
        dlambda - float : variation du parametre par seconde
        ips -int : nombre d'images par seconde

    Retour :
        list of float : parametres

    Exemple :
        déplacement à 1 metre par seconde, d'une durée totale de 2 secondes,
	à 25 images par secondes :
            >>> parametresAnimation(2, 1.0, 25)
        renvoie une liste de 50 valeurs (pour chacune des 50 images nécessaires)
        [0.0, 0.04, 0.08, 0.12, ..., 1.88, 1.92, 1.96]
    """

    return map(lambda a : a * float(dlambda) / ips, range(int(t * ips)))

def calculerScenes (original, n, sDossier = "scenesAnimation", mt = 2) :
    """Lance consécutivement le calcul des fichiers 
	contenus dans le dossier donné en argument

    ====== Attention ======
    Placez ce script dans le répertoire d'installation de Virtuelium
    ou modifiez le chemin d'accés dans la dernière ligne de la fonction.

    Arguments :
        original - str : chemin du fichier original
        n - int : nombre de fichiers
        sDossier - str : (facultatif) sous dossier (par rapport à l'original)
					où sont stockés les fichiers scénes
        mt - int : (facultatif) paramètre mt pour le lancement de Virtuelium
     """
    
    for i in range(n) :
        name = _editerNom(original, sDossier, str(i)) 
        print "Calcul de " + name
	system("virtuelium -n " + str(mt) + " " + name)
    
