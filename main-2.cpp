#include <iostream>
#include <vector>
#include <string>
#include <fstream> // pour les lecture-ecriture dans les fichiers

/**attention : commencer par "o" puis "o" pour creer un nouveau fichier
description du code : c'est une résolution partielle de sudoku (semi-automatisée)
Le but est d’automatiser partiellement la recherche d’une solution à une grille de sudoku 
en programmant quelques stratégies simples de remplissage basées sur des choix forcés pour certaines cases. 
En combinant ces stratégies et en itérant le procédé on peut plus ou moins compléter des grilles, 
selon leur niveau de difficulté. 
Aller au-delà peut nécessiter d’émettre des hypothèses sur la valeur à placer dans une case, 
hypothèse qui peut ensuite aboutir à une solution complète ou au contraire à une contradiction 
amenant à réfuter l’hypothèse initiale et le remplissage de toutes les cases qui en découlait. 
Pour les grilles les plus complexes, il faut enchaîner plusieurs hypothèses avant d’aboutir à une solution ou à un blocage.
**/

using namespace std;

typedef struct
{
    string nom;
    vector<vector<int>> grille;
} Grille;

typedef struct
{
    size_t lig;
    size_t col;
} Indices;

void afficheTab(vector<int> &t)
{
    for (int i = 0; i < t.size(); i++)
    {
        cout << " | " << t.at(i) << " | " << endl;
    }
}

// insérer ici les fonctions que vous appelez avant que leurs definitions
// apparaissent dans le fichier

void imprime(const Grille &g); // definie en fin de fichier.
bool bienFormee(const Grille &g);
bool valide(const Grille &g);
Indices indiceRegionHG (const Grille &g, Indices ind);
vector<int> possibles(const Grille &g, Indices ind);
bool force(Grille &g, Indices ij, vector<int> vpossibles);
bool userSuggest(Grille &g);
bool joue(Grille &g);

/** Renvoie true ssi la grille a la bonne forme et ne contient que des valeurs
 * de 0 à 9
 * sert à valider le format d'une grille
 **/
bool bienFormee(const Grille &g)
{
    bool res = true;
    int nbLigne = 0;
    for (size_t i = 0; i < g.grille.size(); i++)
    {
        if (g.grille.at(i).size() != 9) // vérifie le nombre de colonnes dans une ligne
        {
            res = false;
        }
        nbLigne++; //compte le nombre de lignes
    }
    if (nbLigne != 9) // vérifie le nombre de lignes
    {
        res = false;
        cout << "grille mal formee" << endl;
    }

    //vérification des valeurs
    for(size_t lig = 0; lig < 9; lig ++)
    {
        for(size_t col = 0; col < 9; col ++)
        {
            if (g.grille.at(lig).at(col)<0 or g.grille.at(lig).at(col)>9)
            {
                res = false;
                cout << "grille mal formee" << endl;
            }
        }
    }
    return res;
}


/** Verifie si la grille est valide: chaque valeur de 1 à 9 apparait au plus une fois dans chaque ligne, colonne et région
 * sert aussi a tester si un placement de valeur est correct : on place la valeur, on appelle la fonction sur la nouvelle grille
 * si false -> annuler le placement et essayer autre chose
 **/
bool valide(const Grille &g)
{
    bool res = true;
    int val;
    if (bienFormee(g)==true)
    {
        // vérifier si le nb apparaît au plus une fois dans la ligne: on compare les valeurs entre elles
        for (int lig = 0; lig < 9; lig++)
        {
            for (int col = 0; col < 9; col++)
            {
                if(g.grille.at(lig).at(col) == 0)
                {
                    continue;
                }
                else
                {
                    val = g.grille.at(lig).at(col);
                    for (int c = col+1; c < 9; c++) //on saute la colonne courante pour ne pas la comparer avec elle-même
                    {
                        if(g.grille.at(lig).at(c) == val)
                        {
                            res = false;
                            return res;
                        }
                    }
                }
            }
        }

        //pour les colonnes
        for(int col = 0; col < 9; col++ )
        {
            for (int lig = 0; lig < 9; lig++)
            {
                if (g.grille.at(lig).at(col) == 0)
                {
                    continue;
                }
                else
                {
                    val = g.grille.at(lig).at(col);
                    for (int l = lig+1; l < 9; l++ )
                    {
                        if(g.grille.at(l).at(col) == val)
                        {
                            res = false;
                            return res;
                        }
                    }
                }
            }
        }

        //pour les régions
        int ind_x, ind_y;
        for (int lig = 0; lig < 9; lig++)
        {
            for(int col = 0; col < 9; col++ )
            {
                if(g.grille.at(lig).at(col) == 0)
                {
                    continue;
                }
                else // la case n'est pas vide, il faut donc comparer les valeurs
                {
                    val = g.grille.at(lig).at(col);
                    ind_x = (lig/3)*3;
                    ind_y = (col/3)*3;
                    for (int x = ind_x; x < ind_x+3; x++)// on parcourt la région
                    {
                        for(int y = ind_y; y < ind_y+3; y++)
                            {
                                // quand on est à la case courante, on la saute pour ne pas la comparer avec elle-même
                                if (x!=lig and y!=col and g.grille.at(x).at(y) == val)
                                {
                                    res = false;
                                    return res;
                                }
                                else
                                {
                                    continue;
                                }
                            }
                    }
                }

            }
        }
        return true;

    }
    else
    {
        return false;
    }
}
/**cherche le coin superieur gauche*/
Indices indiceRegionHG (const Grille &g, Indices ind)
{
    Indices coinHG;
    if (ind.lig >= 0 and ind.lig <= 2)
    {
        if(ind.col >= 0 and ind.col <= 2)
        {
            coinHG.lig = 0;
            coinHG.col = 0;
        }
        if(ind.col >= 3 and ind.col <= 5)
        {
            coinHG.lig = 0;
            coinHG.col = 3;
        }
        if(ind.col >= 6 and ind.col <= 8)
        {
            coinHG.lig = 0;
            coinHG.col = 6;
        }
    }
    if (ind.lig >= 3 and ind.lig <= 5)
    {
        if(ind.col >= 0 and ind.col <= 2)
        {
            coinHG.lig = 3;
            coinHG.col = 0;
        }
        if(ind.col >= 3 and ind.col <= 5)
        {
            coinHG.lig = 3;
            coinHG.col = 3;
        }
        if(ind.col >= 6 and ind.col <= 8)
        {
            coinHG.lig = 3;
            coinHG.col = 6;
        }
    }
    if (ind.lig >= 6 and ind.lig <= 8)
    {
        if(ind.col >= 0 and ind.col <= 2)
        {
            coinHG.lig = 6;
            coinHG.col = 0;
        }
        if(ind.col >= 3 and ind.col <= 5)
        {
            coinHG.lig = 6;
            coinHG.col = 3;
        }
        if(ind.col >= 6 and ind.col <= 8)
        {
            coinHG.lig = 6;
            coinHG.col = 6;
        }
    }

    return coinHG;
}



/** renvoie le vecteur des valeurs disponibles pour le couple d'indices ind,
 * compte-tenu des valeurs placées sur sa ligne, colonne et sa région
*/
vector<int> possibles(const Grille &g, Indices ind)
{
    vector<int> res;
    res.clear();
    vector<bool> p (9,true);
    int n;
    //cherche les valeurs déjà existantes
   for (size_t lig = 0; lig < 9; lig++)
    {
        n = g.grille.at(lig).at(ind.col);
        if(n!= 0)
        {
            p.at(n-1) = false;
        }
    }
    for (size_t col = 0; col < 9; col++)
    {
        n = g.grille.at(ind.lig).at(col);
        if (n != 0)
        {
            p.at(n-1) = false;
        }
    }

    Indices coin;
    coin = indiceRegionHG(g,ind);
    size_t x = coin.lig;
    size_t y = coin.col;
    for (size_t lig_reg = x; lig_reg < x+3; lig_reg++)
    {
        for (size_t col_reg = y; col_reg < y+3; col_reg++)
        {
            n = g.grille.at(lig_reg).at(col_reg);
            if (n != 0)
            {
                p.at(n-1) = false;
            }
        }
    }
    //convertir les bool true en int
    for (int i = 0; i < 9; i++)// on balaye le vecteur p
    {
        if(p.at(i) == true)
        {
            res.push_back(i+1);
        }
    }
    return res;
}

/** recoit en argument le vecteur des valeurs possibles pour la case
 * d'indices ij. Parcourt les valeurs en cherchant si pour l'une d'entre
 * elles il existe un seul emplacement valide dans la région.
 * Dans ce cas, stocke la valeur dans la case et renvoie true.
 * Renvoie false si pour chaque valeur possible il existe au moins deux
 * cases où on pourrait potentiellement la ranger. Dans ce cas ne
 * modifie pas la grille.
 **/
bool force(Grille &g, Indices ij, vector<int> vpossibles)
{
    bool res = false;
    int nb;
    Indices coin;
    coin = indiceRegionHG(g,ij);
    size_t x = coin.lig;
    size_t y = coin.col;

    if (vpossibles.size() == 1) //il n'y a qu'une seule valeur possible, c'est donc forcement la bonne
    {
        g.grille.at(ij.lig).at(ij.col) = vpossibles.at(0);
        res = true;
        return res;
    }
    else
    {
        int cptr = 0;
        for (int i =0; i < vpossibles.size(); i++)
        {
            //cout << "valeur testee en cours: " << vpossibles.at(i) << endl;
            for (size_t ii = x; ii < x+3; ii++)//on parcourt la région
            {
                for(size_t jj = y; jj < y+3; jj++)
                {
                    Indices xy = {ii, jj};
                    //cout << "case region testee: " << xy.lig << " , " << xy.col << endl;

                    if(ii != ij.lig or jj != ij.col)//on ne veut pas tester la case courante
                    {
                        if(g.grille.at(ii).at(jj) == 0)
                        {
                            vector<int> v = possibles(g, xy); // vector de la case vide contenant les valeurs possibles (le possible des autres cases)
                            /*cout << "indices force en cours: " << xy.lig << " , " << xy.col << endl;
                            afficheTab(v);
                            cout << endl;*/

                            for (size_t n = 0; n < v.size(); n++)// on parcourt le vecteur possible de chaque case vide
                            {
                                //cout << "valeur testee: " << v.at(n) << endl;
                                if(v.at(n) == vpossibles.at(i))// on compare chaque élément de vpossibles avec les éléments de la case ij qui sont dans vide
                                {
                                    cptr = cptr + 1;// si un nombre est dans les deux vecteurs, il existe dans deux cases, donc on renvoie false
                                    res = false;

                                    //cout << "valeur : " << v.at(n) << " impossible " << endl;
                                }

                            }
                        }
                    }

                }
            }
            if (cptr == 0)//on force la valeur
            {
                g.grille.at(ij.lig).at(ij.col) = vpossibles.at(i);
                res = true;
                //cout << "valeur :" << vpossibles.at(i) << " forcee" << endl;
                return res;
            }
            cptr = 0;//on reinitialise le compteur à 0 pour les autres valeurs de vpossibles
        }
    }
    return res;
}




/** demande à l'uitlisateur de fournir une valeur pour un indice (une case)
*fonction vérifie si la case est vide et que la valeur fournie est possible => modifie g et revoit true
*@param grille g
*@return vrai ou faux si la valeur de l'utilisateur n'est pas bonne
**/

//defini en dehors des fonctions car on en a besoin partout
string sauvegarde;
bool is_sauvegarde = false;
bool is_replay = false;
fstream fichier;

bool userSuggest(Grille &g)
{
    bool res = false;
    int val;
    int cptr = 0;
    char d, r;

    do
    {
        cout << "rentrer le nombre de la ligne: ";
        size_t lig;
        cin >> lig;
        if(lig < 1 or lig > 9) //on teste les indices lignes
        {
            cout << "indice de ligne hors domaine (1-9)" << endl;
            return false;
        }

        cout << "rentrer le nombre de la colonne: ";
        size_t col ;
        cin >> col;
        if(col < 1 or col > 9)//on teste les indices colonnes
        {
            cout << "indice de colonne hors domaine (1-9)" << endl;
            return false;
        }

        // vérifier que la case est vide
        Indices cases;
        cases.lig = lig - 1;
        cases.col = col - 1;
        vector<int> v;
        v = possibles(g,cases);
        if (v.size() == 0)//pas de solutions
        {
            cout << "pas de solution possible " << endl;
            return false;
        }
        if (g.grille.at(cases.lig).at(cases.col) == 0)
        {
            do
            {
                cout << "rentrer la valeur: (0 pour avoir les hypothèses)";
                cin >> val;
                if( val == 0)
                {
                    cout << "voici les hypothèses: " << endl;
                    afficheTab(v);
                    cout << "rentrer votre choix: ";
                    cin >> val;
                }

                g.grille.at(cases.lig).at(cases.col) = val;
                if (valide(g) == true)
                {
                    if(is_sauvegarde == true)
                    {
                        sauvegarde += to_string(cases.lig)+" "+to_string(cases.col)+" "+to_string(val)+"\n";//on mémorise le choix de la cases courante
                    }

                    imprime(g);
                    return true;

                }
                else//si jamais l'utilisateur n'a pas demandé d'indice et qu'il met une valeur fausse
                {
                    g.grille.at(cases.lig).at(cases.col) = 0;
                    cout << "valeur incorrecte, voulez-vous rentrer une autre valeur? (o ou n): ";
                    cin >> d;

                    if (d == 'o')
                    {
                        imprime(g);//pour remontrer à l'utilisateur la grille
                        cout << "voici les hypothèses: " << endl;
                        afficheTab(v);
                        cptr = cptr + 1;
                    }
                    else if (d == 'n')
                    {
                        //cout << "je n'ai pas choisi une autre valeur" << endl;
                        if (is_sauvegarde)//il faut sauvegarder les valeurs avant de sortir de la fonction
                        {
                            fichier.open(g.nom+"_sauvegarde.txt", ios::app);
                            fichier << sauvegarde;
                            fichier.close();
                        }
                        //cout << "je quitte userSuggest" << endl;
                        return false;
                    }

                }
            } while(d == 'o' or cptr < 3);

        }
        else
        {
            cout << "case deja remplie, voulez-vous rentrer un autre indice? (c pour continuer ou q pour quitter)";
            cin >> r;
            if (r == 'q')
            {
                cout << "j'ai demande de quitter " << endl;
                cout << r << endl;
                r = 'q';

                if (is_sauvegarde)//il faut sauvegarder les valeurs avant de sortir de la fonction
                {
                    fichier.open(g.nom+"_sauvegarde.txt", ios::app);
                    fichier << sauvegarde;
                    fichier.close();
                }
                return false;
            }
        }

    }while (r == 'c');
}

bool resJoue = false;//on le sort de la fonction joue pour ne pas qu'il se re-initialise quand on rappelle joue
bool is_break = false;

bool joue(Grille &g)
{
    cout << "Debut du traitement de la grille " << g.nom << endl;
    imprime(g);

    int i, j, val;//pour la recuperation des entiers dans le fichier de sauvegarde
    int cptrBoucle = 0;
    vector<int> v;
    Indices ind;
    int cptrGrille1 =81;
    int cptrGrille2 = 0;
    int cptrFin;

    if (is_replay)//si on veut rejouer une partie, il faut completer la grille avec les valeurs sauvegardees
    {
        cout << "chargement des valeurs sauvegardees" << endl;
        ifstream fichier(g.nom+"_sauvegarde.txt");
        string ligne;
        while(getline(fichier, ligne)) //tant qu'on n'est pas à la fin du fichier, on lit
       {
           //cout << ligne << endl;
           i = ligne[0]- '0';// pour convertir en entier
           j = ligne [2]- '0';
           val = ligne[4]- '0';
           g.grille.at(i).at(j) = val;
           //cout << i << " , " << j << " , " << val << endl;

       }
        fichier.close();
    }
    while (cptrGrille1 -  cptrGrille2 != 0)//tant que la grille n'est pas complète
    {
        cptrBoucle += 1;//pour savoir combien de boucle le programme fait avant de terminer la grille
        int cptrTemp1 = 0;
        cptrGrille1 = 81;// il faut réinitialiser le cptr
        for (size_t l = 0; l < 9; l++)
        {
            for (size_t c = 0; c < 9; c++)
            {
                if(g.grille.at(l).at(c) !=0)//compte les cases occupées
                {
                    cptrGrille1 = cptrGrille1 - 1;//nombre de cases libres
                }
                if(g.grille.at(l).at(c) ==0)//compte les cases vides
                {
                    cptrTemp1 += 1;//nombre de cases libres
                }
            }
        }
        /*cout << "cptrGrille1 = " << cptrGrille1 << endl;
        cout << "cptrTemp1 = " << cptrTemp1 << endl;
        cout << endl;*/

        for (size_t lig = 0; lig < 9; lig++) // on parcourt la grille
        {
            for (size_t  col = 0; col < 9; col++)
            {
                if (g.grille.at(lig).at(col) == 0)
                {
                    ind = {lig,col};
                    //cout << ind.lig << " , " << ind.col << endl;

                    v = possibles(g,ind);// on a les valeurs possible de la case courante

                    if (force(g,ind,v) == true) //on teste force
                    {
                        //cout << "ind ap force: " << ind.lig << " , " << ind.col << endl;
                        if (valide(g) == true)
                        {
                            continue;
                        }
                        //cout << "ind ap valide: " << ind.lig << " , " << ind.col << endl;
                    }
                    else
                    {
                        g.grille.at(lig).at(col) = 0;
                    }
;
                }//fin if

            }// fin for col
        }// fin for lig

        int cptrTemp2 = 0;//on recompte le nombre de place vide après force
        for (size_t l = 0; l < 9; l++)
        {
            for (size_t c = 0; c < 9; c++)
            {
                if(g.grille.at(l).at(c) ==0)//compte les places vides
                {
                    cptrTemp2 += 1;
                }
            }
        }
        /*cout << "cptrTemp2 = " << cptrTemp2 << endl;
        cout << endl;*/
        if(cptrTemp1 == cptrTemp2)
        {
            cptrGrille2 = cptrGrille1;//on sort du while, toutes les cases ont été remplies
        }
        else
        {
            cptrGrille2 = cptrGrille1 - cptrTemp2;//le nombre de cases qu'il reste à remplir
        }
        /*cout << "cptrGrille2 = " << cptrGrille2 << endl;
        cout << endl;*/

        cout << "Passage " << cptrBoucle << " : ";
        cout << "nombre de cases qu'il reste a completer : " << cptrTemp2 << endl;
        cptrFin = cptrTemp2;
        //cout << " sortie while cptrFin: "<< cptrFin << endl;

    }//fin while

    cout << endl;
    imprime(g);

    cout << "il reste " << cptrFin << " cases a completer" << endl;
    if (cptrFin != 0)//il y a encore des cases vides à compléter
    {
        userSuggest(g);
        //cout << "sortie userSuggest" << endl;

        if (userSuggest(g) == false)
        {
            if (is_sauvegarde)//il faut sauvegarder la valeur
            {
                fichier.open(g.nom+"_sauvegarde.txt", ios::app);
                fichier << sauvegarde;
                fichier.close();

            }
            resJoue = true;

        }
        else
        {

            if (valide(g) == true)
            {
                //cout << "Je continue" << endl;
                joue(g);
            }
            else
            {
                resJoue = true;

            }
        }
    }

    //verifier si toutes les cases sont remplies
    /*for (int lig = 0; lig < 9; lig++)
    {
        for (int col =0; col < 9; col++)
        {
            if (g.grille.at(lig).at(col) == 0)
            {
                return false;
            }
            else
            {
                valide(g);
                if (valide(g) == true)
                {
                    return true;
                }
            }
        }
    }*/
    return resJoue;
}





/* quelques grilles prédéfinies */
int main() {

    //test bienFormee (exemple1/7)
    /*Grille g1 = { "G1",
		{ { 0, 2, 7, 0, 6, 5, 0, 4, 9 },
		  { 3, 5, 0, 0, 0, 9, 0, 0, 8 },
		  { 8, 9, 0, 0, 0, 0, 0, 0, 0 },
		  { 2, 0, 0, 0, 9, 0, 0, 0, 0 },
		  { 0, 0, 0, 6, 8, 7, 0, 0, 0 },
		  { 0, 0, 0, 0, 3, 0, 0, 0, 5 },
		  { 0, 0, 0, 0, 0, 0, 0, 6, 3 },
		  { 7, 0, 0, 2, 0, 0, 0, 8, 4 },
		  //{ 4, 8, 0, 9, 7, 0, 1, 5, 0}
		}};
		bienFormee(g1);*/

  // Les grilles G2 à G3 peuvent se résoudre sans aide de l'utilisateur
  Grille g2 = { "G2",
		{ { 5, 3, 0, 0, 7, 0, 0, 0, 0 },
		  { 6, 0, 0, 1, 9, 5, 0, 0, 0 },
		  { 0, 9, 8, 0, 0, 0, 0, 6, 0 },
		  { 8, 0, 0, 0, 6, 0, 0, 0, 3 },
		  { 4, 0, 0, 8, 0, 3, 0, 0, 1 },
		  { 7, 0, 0, 0, 2, 0, 0, 0, 6 },
		  { 0, 6, 0, 0, 0, 0, 2, 8, 0 },
		  { 0, 0, 0, 4, 1, 9, 0, 0, 5 },
		  { 0, 0, 0, 0, 8, 0, 0, 7, 9 }
		}};

        //test force (exemple3/7)
		/*Indices ind;
		//ind = {1,3};//ne remplie pas tout de suite la case
		//ind = {4,4};
		vector<int> v;
		v = possibles(g2, ind);
		afficheTab(v);
		force(g2,ind,v);//la fonction force ne complète pas la grille de manière chronologique mais de manière synthétique (comme un vrai joueur)
		imprime(g2);*/

  Grille g3 = { "G3",
		{ { 0, 2, 7, 0, 6, 5, 0, 4, 9 },
		  { 3, 5, 0, 0, 0, 9, 0, 0, 8 },
		  { 8, 9, 0, 0, 0, 0, 0, 0, 0 },
		  { 2, 0, 0, 0, 9, 0, 0, 0, 0 },
		  { 0, 0, 0, 6, 8, 7, 0, 0, 0 },
		  { 0, 0, 0, 0, 3, 0, 0, 0, 5 },
		  { 0, 0, 0, 0, 0, 0, 0, 6, 3 },
		  { 7, 0, 0, 2, 0, 0, 0, 8, 4 },
		  { 4, 8, 0, 9, 7, 0, 1, 5, 0}
		}};
		//test userSuggest(exemple2/7)
		/*imprime(g3);
		userSuggest(g3);*/


  Grille g4 = { "G4",
		{ { 8, 0, 0, 0, 0, 0, 1, 0, 0 },
		  { 0, 3, 4, 0, 2, 0, 6, 0, 0 },
		  { 0, 9, 0, 4, 0, 0, 0, 0, 2 },
		  { 5, 1, 0, 0, 4, 2, 9, 6, 0 },
		  { 0, 0, 0, 0, 6, 0, 0, 0, 0 },
		  { 0, 6, 7, 1, 5, 0, 0, 2, 3 },
		  { 9, 0, 0, 0, 0, 4, 0, 3, 0 },
		  { 0, 0, 2, 0, 1, 0, 7, 4, 0 },
		  { 0, 0, 8, 0, 0, 0, 0, 0, 1 }
		}};
		//test de joue (exemple4/7)
		//joue(g4);

  // cette grille admet deux solutions différentes mais demande une aide
  // de l'utilisateur si on se limite aux deux autres stratégies définies
  Grille g5 = { "G5",
		{ { 0, 0, 8, 0, 0, 0, 2, 0, 0 },
		  { 0, 0, 4, 3, 0, 0, 0, 0, 1 },
		  { 0, 5, 0, 0, 0, 7, 0, 8, 9 },
		  { 0, 0, 5, 7, 4, 0, 0, 0, 2 },
		  { 0, 0, 0, 1, 0, 3, 0, 0, 0 },
		  { 4, 0, 0, 0, 2, 0, 6, 0, 0 },   // deux solutions possibles: indice{0,7} soit 5, soit 6
		  { 2, 6, 0, 5, 0, 0, 0, 7, 0 },   // deux solutions {5,2}, soit 3 soit 7;
		  { 5, 0, 0, 0, 0, 2, 9, 0, 0 },
		  { 0, 0, 1, 0, 0, 0, 5, 0, 0 }
		}};

  //joue(g2);
  //joue(g3);
  //joue(g4);
  char d1, d2;
  sauvegarde.clear();
  fstream monFichier("G5_sauvegarde.txt");

  cout << "voulez-vous rejouer la partie ? (o ou n) ";
  cin >> d1;
  if (d1 == 'o')
  {
        is_replay = true;


        if (monFichier)//vérifie si le fichier existe
        {
            cout << "fichier de sauvegarde trouve" << endl;
            monFichier.close();//pour etre sur qu'il soit ferme avant reouverture plus tard
        }
        else
        {
            is_replay = false;
            cout << "ERREUR: Impossible d'ouvrir le fichier en lecture." << endl;
        }


  }

  cout << "voulez-vous sauvegarder vos choix ? (o ou n) ";
  cin >> d2;
    if(d2 == 'o')
    {
        is_sauvegarde = true;
    }
  joue(g5);

  return 0;
}

		// Fonctions fournies, rien à modifier a priori.


/** Imprime une ligne de séparation formée du caractère passé en argument */
void imprimeL(char c) {
  cout << ' ';
  for(size_t i = 0; i < 9; i += 1) { cout << c << c << c << c; }
  /* pour prendre en compte les espaces ajoutés prèsb des '+' */
  cout << c << c << c << c << c << c;
  cout << endl;
}

/** suppose la grille bien formee: 9 lignes de 9 colonnes de chiffres de 0 à 9 */
void imprime(const Grille &g) {
  for(size_t i = 0; i < 9; i += 1) {
    imprimeL(i % 3 == 0 ? '+' : '-'); // ligne d'en-tete
    for(size_t j = 0; j < 9; j += 1) {
      char c = '0' + g.grille.at(i).at(j);
      cout << (j % 3 == 0 ? " + " : " " ) << ' ' << c << ' ';
    }
    cout << "+" << endl;
  }
  imprimeL('+'); // ligne finale du bas
  cout << endl;
}
