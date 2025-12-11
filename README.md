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
