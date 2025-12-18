# Compte rendu TP1

# Compilation
Pour compiler une question `qx` (où `qx` est par ex. `q1.c`) utilise la commande suivante :

```
bash
gcc -std=c11 -Wall -Wextra -O2 utils.c qx.c -o qx.
```

## question1
<img width="345" height="93" alt="image1" src="https://github.com/user-attachments/assets/c24128b1-dd9d-4f0a-99c4-50c3cb35ed57" />


STDOUT_FILENO est le descripteur système de la sortie standard utilisé avec write() pour un affichage direct sans buffering, contrairement à l’utilisation classique de stdout c'est pire encore avec printf
Dans cette question on affiche simplement le message dans le shell.

```
display_welcome() //pour afficher le message de bienvenue.

write_str(PROMPT) //pour afficher le prompt.

```


## question2
<img width="540" height="157" alt="image2" src="https://github.com/user-attachments/assets/2aed590d-8095-4adb-87c1-b08ca6c64629" />
<img width="566" height="293" alt="image6" src="https://github.com/user-attachments/assets/81382f71-a9d9-4d41-b4c8-67732cbbea0d" />

L’exécution suit le modèle REPL : le shell lit la commande saisie avec read(STDIN_FILENO, buf, BUF_SIZE-1), la nettoie avec trim_newline(), crée un processus enfant avec fork(), exécute la commande via execvp(), et termine l’enfant avec _exit(127) si la commande est introuvable.


## question3
<img width="540" height="135" alt="image3" src="https://github.com/user-attachments/assets/9e089e57-9348-4a75-9b92-9538a62c54b5" />

Le shell lit les commandes saisies par l’utilisateur et les nettoie avec trim_newline(). Si l’utilisateur tape exit ou Ctrl+D, il affiche un message d’au revoir et se termine. Pour toute autre commande, il crée un processus enfant avec fork() qui exécute la commande via execvp(). Si la commande est introuvable, l’enfant termine avec _exit(127). Le parent attend la fin de l’enfant avec wait(NULL) avant de réafficher le prompt, permettant ainsi un cycle REPL (Read–Eval–Print Loop) pour traiter les commandes successives.

## question4
<img width="785" height="182" alt="image4" src="https://github.com/user-attachments/assets/7b6dbfeb-9647-457f-b1eb-435d2e4f0567" />
<img width="910" height="591" alt="image8" src="https://github.com/user-attachments/assets/05d98d5e-cf3b-447d-9aba-86c5f78cc8f4" />
Le programme boucle en mode REPL : il lit une ligne depuis STDIN_FILENO, la nettoie (trim_newline) et ignore les lignes vides ou Ctrl+D (qui provoque display_goodbye() et la sortie). Si l’utilisateur tape exit le shell quitte proprement. Pour toute autre commande il crée un enfant (fork()), qui exécute soit une commande interne spéciale segfault (force un SIGSEGV), soit un programme externe via execvp() (avec _exit(127) si execvp échoue). Le parent attend la fin de l’enfant (waitpid) puis met à jour has_status, last_exit (avec WEXITSTATUS si l’enfant s’est terminé normalement) ou last_signal (avec WTERMSIG si l’enfant a été tué par un signal). Ces valeurs sont utilisées au prochain affichage du prompt pour montrer le code de retour ou le signal de la commande précédente.

## question5
<img width="502" height="159" alt="image5" src="https://github.com/user-attachments/assets/29d248f4-b409-4cb2-9208-07f7e9f67461" />
<img width="775" height="300" alt="image7" src="https://github.com/user-attachments/assets/0eaf2cf8-e66c-48be-bc78-0a05aed9eff0" />
Pour cette version du shell, nous avons activé les fonctionnalités POSIX en ajoutant #define _POSIX_C_SOURCE 199309L, ce qui permet d’utiliser clock_gettime pour mesurer précisément le temps d’exécution des commandes. Le shell fonctionne toujours en boucle REPL : il lit la commande, la nettoie avec trim_newline(), ignore les lignes vides, et quitte proprement en cas de exit ou de Ctrl+D. Pour chaque commande non interne, le shell crée un processus enfant avec fork(). Le parent enregistre l’instant avant waitpid et après la fin de l’enfant pour calculer la durée d’exécution en millisecondes. L’enfant exécute la commande avec execvp(), ou provoque volontairement un SIGSEGV si la commande est segfault. Après l’attente de l’enfant, le parent met à jour last_exit si l’enfant s’est terminé normalement, ou last_signal si l’enfant a été tué par un signal. La durée calculée (exec_time_ms) est ensuite affichée dans le prompt via display_prompt_q5, donnant par exemple enseash [exit:0|123ms] %, ce qui permet à l’utilisateur de voir en temps réel le code de retour et le temps d’exécution de chaque commande.

## question6
<img width="531" height="578" alt="image9" src="https://github.com/user-attachments/assets/ad2c8b6d-9180-4d55-893f-63998bf77591" />
<img width="1298" height="708" alt="Screenshot from 2025-12-18 20-51-56" src="https://github.com/user-attachments/assets/77c9da6c-8bf7-4da5-9521-3ab5105bc019" />

La commande saisie est découpée en arguments à l’aide de strtok_r, où les espaces et tabulations jouent le rôle de séparateurs. La tokenisation consiste à parcourir la chaîne et à remplacer chaque séparateur par '\0', ce qui transforme la ligne en plusieurs sous-chaînes indépendantes. Cette opération est réalisée directement dans le buffer buf,  les pointeurs du tableau argv_exec référencent simplement les débuts de ces sous-chaînes à l’intérieur du buffer. La taille de argv_exec est choisie comme BUF_SIZE / 2 + 1 qui est le nombre maximal d’arguments possibles (arguments d’un caractère séparés par un espace). Le tableau est terminé par NULL, pour l’appel à execvp, permettant ainsi l’exécution correcte de commandes avec arguments.

## question7
<img width="531" height="665" alt="image10" src="https://github.com/user-attachments/assets/ee2a5b32-9d08-427c-89ef-91af6dc25b01" />
<img width="1295" height="767" alt="Screenshot from 2025-12-18 20-53-00" src="https://github.com/user-attachments/assets/2391f9a6-d473-4810-8d6c-6f4c6e46b98b" />
<img width="1199" height="730" alt="image" src="https://github.com/user-attachments/assets/b97cf7f4-5202-4665-98db-57fe3c4df71d" />
<img width="1334" height="776" alt="Screenshot from 2025-12-18 20-54-20" src="https://github.com/user-attachments/assets/4c4263e0-642b-448d-b67a-77b8bde33c56" />


Après la tokenisation de la ligne de commande, le programme parcourt les différents mots afin de détecter les symboles de redirection > et <. Lorsqu’un > est rencontré, le mot suivant est interprété comme le nom du fichier de sortie, tandis qu’un < indique un fichier d’entrée. Ces éléments ne doivent pas être transmis au programme exécuté, c’est pourquoi une nouvelle liste d’arguments (argv_clean) est construite en excluant les symboles de redirection et leurs noms de fichiers. Dans le processus enfant, les fichiers indiqués sont ouverts avec open (en écriture pour >, en lecture pour <) et les appels à dup2 permettent de remplacer respectivement la sortie standard (STDOUT_FILENO) ou l’entrée standard (STDIN_FILENO) par ces fichiers. Une fois les redirections mises en place, l’appel à execvp exécute la commande avec les arguments nettoyés ; le programme lancé se comporte alors comme si la redirection avait été faite directement depuis le terminal.


## question8
<img width="531" height="141" alt="image11" src="https://github.com/user-attachments/assets/986a6f2a-e801-4dec-9b50-beac171a7dec" />
<img width="1334" height="776" alt="Screenshot from 2025-12-18 20-55-19" src="https://github.com/user-attachments/assets/3288a149-f42a-4abf-885f-425e1afbfb89" />


Lorsqu’une commande avec un pipe est détectée, le programme commence par analyser la ligne de commande afin d’identifier la position du symbole |, qui sépare les deux commandes à exécuter. Le tableau d’arguments est alors scindé en deux parties, chacune formant une liste d’arguments valide destinée à un appel à execvp. Un pipe est ensuite créé à l’aide de la fonction pipe(), qui fournit deux descripteurs de fichiers : l’un pour l’écriture et l’autre pour la lecture. Deux appels à fork() sont nécessaires afin de créer deux processus fils indépendants. Le premier fils redirige sa sortie standard vers l’extrémité d’écriture du pipe grâce à dup2, ce qui permet à tout ce qu’il affiche d’être envoyé dans le pipe, puis il exécute la commande de gauche avec execvp. Le second fils redirige son entrée standard depuis l’extrémité de lecture du pipe, ce qui lui permet de lire les données produites par la première commande, avant d’exécuter la commande de droite. Le processus parent ferme les descripteurs du pipe devenus inutiles et attend la terminaison des deux processus fils avec waitpid.
