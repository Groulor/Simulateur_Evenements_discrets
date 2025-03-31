#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int X0 = 7;

double U01(void)
{
    int a = 167, b = 0, c = 4352;
    double R, X;
    X = ((a * X0 + b) % c);
    X0 = X;
    R = X / c;
    return R;
}

double UAB(double A, double B)
{
    double R;
    R = A + (B - A) * U01();
    return R;
}

double Expo(float lambda)
{
    double X, R;
    R = U01();
    X = (-1 / lambda) * log(R);
    return X;
}

double Normal01()
{
    double R, somme, X, n = 200;
    somme = 0;
    for (int i = 0; i < n; i++)
    {
        R = U01();
        somme = somme + R;
    }
    X = ((sqrt(12) / sqrt(n)) * (somme - (float)(n / 2)));
    return X;
}

double Normal(float moy, float ecart)
{
    return Normal01() * ecart + moy;
}

/* Type 1 = quand agv1 arrive à la prod
Type 2 = quand agv1 part de la prod
Type 3 = quand agv1 arrive au warehouse
Type 4 = quand agv1 part du warehouse
Type 5 = quand agv 2 arrive au warehouse
Type 6 = quand agv 2 part du warehouse
Type 7 = quand agv 2 arrive au client
Type 8 = quand agv 2 part du client
*/
float t = 0; //0 indique disponible et 1 indique occupé (valable pour l'ensemble du programme)
int agv1 = 0; // l'état 2 indique que l'agv est en attente
int agv2 = 0;
int H = 10000;
float dateAGV1 = 0;
float dateAGV2 = 2 * 10000;
float typeAGV1 = 1;
float typeAGV2 = 0;
int warehouse = 0;
int nb_piece = 0;

int commandes[64] = {0};
int temps_commande[64] = {0};

int indiceagv2 = 0;
int indiceagv1 = 0;
int stockprod = 64;
int stockclient = 0;

void agv1_arrive_prod()
{
    float duree_prise = 0;
    nb_piece = UAB(1, 6);
    printf("agv1_arrive_prod: nb_piece = %d, stockprod = %d\n", nb_piece, stockprod); // Debug print for nb_piece and stockprod
    if (stockprod > nb_piece)
    {
        for (int i = 1; i <= nb_piece; i++)
        {
            duree_prise += Normal(18.66, 0.48);
        }
        stockprod -= nb_piece;
        commandes[indiceagv1] = nb_piece;
        dateAGV1 = t + duree_prise;
        typeAGV1 = 2;
    }
    else
    {
        nb_piece = UAB(1, stockprod);
        for (int i = 1; i <= nb_piece; i++)
        {
            duree_prise += Normal(18.66, 0.48);
        }
        stockprod -= nb_piece;
        commandes[indiceagv1] = nb_piece;

        dateAGV1 = t + duree_prise;
        typeAGV1 = 2;
    }
    printf("agv1_arrive_prod: dateAGV1 = %f\n", dateAGV1); // Debug print for dateAGV1
}

void agv1_depart_prod()
{
    dateAGV1 = t + Normal(30.35167, 1.761);
    typeAGV1 = 3;
    printf("agv1_depart_prod: dateAGV1 = %f\n", dateAGV1); // Debug print for dateAGV1
}

void agv1_arrive_warehouse()
{
    float duree_pose = 0;
    if (warehouse == 1)
    {
        agv1 = 2;
        dateAGV1 = 2 * H;
    }
    else
    {
        warehouse = 1;
        for (int i = 1; i <= commandes[indiceagv1]; i++)
        {
            duree_pose += Normal(18.32, 0.6803);
        }
        indiceagv1++;
    }
    dateAGV1 = t + duree_pose;
    typeAGV1 = 4;
    printf("agv1_arrive_warehouse: dateAGV1 = %f\n", dateAGV1); // Debug print for dateAGV1
}

void agv1_depart_warehouse()
{
    float duree_prise = 0;
    warehouse = 0;
    if (agv2 == 2)
    {
        warehouse = 1;
        for (int i = 1; i <= commandes[indiceagv2]; i++)
        {
            duree_prise += Normal(18.66, 0.48);
        }
        dateAGV2 = t + duree_prise;
        typeAGV2 = 6;
        printf("agv1_depart_warehouse: dateAGV2 = %f\n", dateAGV2); // Debug print for dateAGV2
    }
    else if (typeAGV2 == 0)
    {
        dateAGV2 = t + Normal(29.74, 0.844);
        typeAGV2 = 5;
        agv2 = 1;
        printf("agv1_depart_warehouse: dateAGV2 = %f\n", dateAGV2); // Debug print for dateAGV2
    }

    if (stockprod > 0)
    {
        dateAGV1 = t + Normal(30.35167, 1.761);
        typeAGV1 = 1;
    }
    else
    {
        dateAGV1 = 10000000000000000000000000000000000;
        agv1 = 2;
    }
    printf("agv1_depart_warehouse: dateAGV1 = %f, stockprod = %d\n", dateAGV1, stockprod); // Debug print for dateAGV1 and stockprod
}

void agv2_arrive_warehouse()
{
    float duree_prise = 0;
    if (warehouse == 1)
    {
        agv2 = 2;
        dateAGV2 = 2 * H;
    }
    else
    {
        warehouse = 1;
    }
    for (int i = 1; i <= commandes[indiceagv2]; i++)
    {
        duree_prise += Normal(18.66, 0.48);
    }

    dateAGV2 = t + duree_prise;
    typeAGV2 = 6;
    printf("agv2_arrive_warehouse: dateAGV2 = %f\n", dateAGV2); // Debug print for dateAGV2
}

void agv2_depart_warehouse()
{
    float duree_pose = 0;
    warehouse = 0;
    if (agv1 == 2)
    {
        warehouse = 1;
        for (int i = 1; i <= commandes[indiceagv1]; i++)
        {
            duree_pose += Normal(18.32, 0.6803);
        }
        indiceagv1++;
        dateAGV1 = t + duree_pose;
        if (stockprod > 0)
        {
            dateAGV1 = t + Normal(30.35167, 1.761);
            typeAGV1 = 1;
        }
        else
        {
            dateAGV1 = 2 * H;
        }
        printf("agv2_depart_warehouse: dateAGV1 = %f\n", dateAGV1); // Debug print for dateAGV1
    }
    dateAGV2 = t + Normal(31.93, 0.6781);
    typeAGV2 = 7;
    printf("agv2_depart_warehouse: dateAGV2 = %f\n", dateAGV2); // Debug print for dateAGV2
}

void agv2_arrive_client()
{
    float duree_pose = 0;
    for (int i = 1; i <= commandes[indiceagv2]; i++)
    {
        duree_pose += Normal(18.32, 0.6803);
    }
    stockclient += commandes[indiceagv2];
    printf("stockclient = %d\n", stockclient); // Debug print for stockclient
    indiceagv2++;
    dateAGV2 = t + duree_pose;
    typeAGV2 = 8;
    printf("agv2_arrive_client: dateAGV2 = %f\n", dateAGV2); // Debug print for dateAGV2
    temps_commande[indiceagv2] = t;
}

void agv2_depart_client()
{
    if (commandes[indiceagv2] != 0)
    {
        dateAGV2 = t + Normal(31.93, 0.6781);
        typeAGV2 = 5;
    }
    else
    {
        dateAGV2 = 2 * H;
        typeAGV2 = 0;
    }
    printf("agv2_depart_client: dateAGV2 = %f, stockclient = %d\n", dateAGV2, stockclient); // Debug print for dateAGV2 and stockclient
}

int main()
{
    while (stockclient < 64)
    {
        if (dateAGV1 < dateAGV2)
        {
            if (typeAGV1 == 1)
            {
                agv1_arrive_prod();
                t = dateAGV1;
                printf("t = %f, stockclient = %d, typeAGV1 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV1, dateAGV1, dateAGV2, agv1);
            }
            else if (typeAGV1 == 2)
            {
                agv1_depart_prod();
                t = dateAGV1;
                printf("t = %f, stockclient = %d, typeAGV1 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV1, dateAGV1, dateAGV2, agv1);
            }
            else if (typeAGV1 == 3)
            {
                agv1_arrive_warehouse();
                t = dateAGV1;
                printf("t = %f, stockclient = %d, typeAGV1 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV1, dateAGV1, dateAGV2, agv1);
            }
            else if (stockprod == 0)
            {
                dateAGV1 = 100000;
            }
            else if (typeAGV1 == 4 & stockprod > 0)
            {
                agv1_depart_warehouse();
                t = dateAGV1;
                printf("t = %f, stockclient = %d, typeAGV1 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV1, dateAGV1, dateAGV2, agv1);
            }
        }
        else
        {
            if (typeAGV2 == 5)
            {
                agv2_arrive_warehouse();
                t = dateAGV2;
                printf("t = %f, stockclient = %d, typeAGV2 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV2, dateAGV1, dateAGV2, agv1);
            }
            else if (typeAGV2 == 6)
            {
                agv2_depart_warehouse();
                t = dateAGV2;
                printf("t = %f, stockclient = %d, typeAGV2 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV2, dateAGV1, dateAGV2, agv1);
            }
            else if (typeAGV2 == 7)
            {
                agv2_arrive_client();
                t = dateAGV2;
                printf("t = %f, stockclient = %d, typeAGV2 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV2, dateAGV1, dateAGV2, agv1);
            }
            else if (typeAGV2 == 8)
            {
                agv2_depart_client();
                t = dateAGV2;
                printf("t = %f, stockclient = %d, typeAGV2 = %f, dateAGV1 = %f, dateAGV2 = %f, agv1 = %d\n", t, stockclient, typeAGV2, dateAGV1, dateAGV2, agv1);
            }
        }
    }
        printf("\n");
    printf("commandes[indiceagv1]=%d stockprod=%d stockclient=%d\n",commandes[indiceagv1], stockprod, stockclient);
    printf("nombre de commandes ou indiceagv1=%d\n", indiceagv1);
    printf("nombre de commandes ou indiceagv2=%d\n",indiceagv2);
            printf("\n");

    printf("Duree totale t= %f  \n", t);
    printf("moyenne du temps passe chez chaque commande =%f\n", t / (indiceagv2-1));
        printf("\n");


      printf("liste des commandes :\n");
    for (int i = 0; i <= indiceagv2; i++) {
        printf("%d ", commandes[i]);
        }
    printf("\n");

    printf("temps des commandes en cumule :\n");
      for (int i = 0; i <= indiceagv2; i++) {
       printf("%d ", temps_commande[i]);
      }
        printf("\n");

   if (indiceagv2 > 0) {
        printf("ecart entre les temps de commande :\n");
        for (int i = 1; i <= indiceagv2; i++) {
            printf("%d ", temps_commande[i] - temps_commande[i - 1]);
        }
        printf("\n");
    }
    return 0;
}
