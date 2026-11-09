#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>


// struct pra salvar as atvidades -- lista simplesmente encadeada --
typedef struct tarefa{
    // info principal
    char titulo[30];
    char materia[30];

    char data[11];
    int data_sist;

    char prazo[11];
    int prazo_sist;

    int nota;

    // no para o proximo
    struct tarefa* proximo;
}tarefa;

// variaveis para deixar global
typedef struct Global{

    GtkWidget *materia;
    GtkWidget *titulo;
    GtkWidget *nota;
    GtkWidget *calendar;
    GtkWidget *stack;

    int Datual;
    int Matual;
    int Aatual;

    tarefa *head;

} Global;

// funçoes outras
void abrir_calendario(GtkWidget *botao, gpointer dados) {
    GtkWidget *popover = GTK_WIDGET(dados);

    gtk_widget_show_all(popover);
    gtk_popover_popup(GTK_POPOVER(popover));
}

static void limpar_campos(gpointer dados) { // dados precisa ser &global
    Global *global=dados;

    gtk_entry_set_text(GTK_ENTRY(global->materia), "");
    gtk_entry_set_text(GTK_ENTRY(global->titulo), "");
    gtk_entry_set_text(GTK_ENTRY(global->nota), "");

}

void criar_atividade(GtkWidget *botao, gpointer dados) {

    Global *global = dados;

    tarefa *novo = malloc(sizeof(tarefa));
    if (novo == NULL) return;

    const char *mat = gtk_entry_get_text(
        GTK_ENTRY(global->materia)
    );

    const char *tit = gtk_entry_get_text(
        GTK_ENTRY(global->titulo)
    );

    const char *nota = gtk_entry_get_text(
        GTK_ENTRY(global->nota)
    );

    guint ano, mes, dia;
    gtk_calendar_get_date(GTK_CALENDAR(global->calendar), &ano, &mes, &dia);

    snprintf(novo->prazo, sizeof(novo->prazo), "%02u/%02u/%04u", dia, mes + 1, ano); // mes +1 porque o sistema le como 11
    novo->prazo_sist = ano*10000 + (mes+1)*100 + dia; 

    snprintf(novo->data, sizeof(novo->data), "%02u/%02u/%04u", global->Datual, global->Matual, global->Aatual); 
    novo->data_sist = global->Aatual*10000 + global->Matual*100 + global->Datual; 

    // exclui o novo caso o prazo seja antes da data atual
    if(novo->prazo_sist<novo->data_sist){
        free(novo);
        return;
    }

    snprintf(novo->materia, sizeof(novo->materia), "%s", mat);
    snprintf(novo->titulo, sizeof(novo->titulo), "%s", tit);

    novo->nota = atoi(nota); // transforma a nota em int

    // empurra pra frente
    novo->proximo = global->head;
    global->head = novo;

    // limpa os campos e volta pro inicio
    limpar_campos(dados);
    GtkStack *stack = GTK_STACK(global->stack);
    gtk_stack_set_visible_child_name(stack, "inicio");
}

// muda a tela para a de criaçao, inicio e ler respectivamente.

void trcriar(GtkWidget *botao, gpointer dados) {
    GtkStack *stack = GTK_STACK(dados);

    gtk_stack_set_visible_child_name(stack, "criar");
}

void trinicio(GtkWidget *botao, gpointer dados) {
    GtkStack *stack = GTK_STACK(dados);

    gtk_stack_set_visible_child_name(stack, "inicio");
}

void trler(GtkWidget *botao, gpointer dados) {
    GtkStack *stack = GTK_STACK(dados);

    gtk_stack_set_visible_child_name(stack, "ler");
}


int main(int argc, char **argv) {

    gtk_init(&argc, &argv);

    // ----- codigo comeca aqui ----- 

    // data atual
    time_t agora = time(NULL);
    struct tm *data = localtime(&agora);

    int Datual = data->tm_mday;
    int Matual = data->tm_mon + 1;
    int Aatual = data->tm_year + 1900;

    // configuracao janela
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window),1260,860);
    gtk_window_set_title(GTK_WINDOW(window), "Agenda escolar" );

    // config do grid Inicio ----0000----
    GtkWidget *inicio;
    inicio = gtk_grid_new();

    // config butoes
    GtkWidget *btc;
    GtkWidget *btl;
    btc=gtk_button_new_with_label("Criar");
    btl=gtk_button_new_with_label("Ler");

    gtk_grid_set_row_spacing(GTK_GRID(inicio), 15);
    gtk_grid_set_column_spacing(GTK_GRID(inicio), 15);
    gtk_widget_set_halign(inicio, GTK_ALIGN_CENTER );
    gtk_widget_set_valign(inicio, GTK_ALIGN_CENTER );

    // config Botoes e grid
    gtk_grid_attach(GTK_GRID(inicio), btc,0,0,1,1);
    gtk_grid_attach(GTK_GRID(inicio), btl,1,0,1,1);

    // config do grid Criar ----0001----
    GtkWidget *criar;
    criar = gtk_grid_new();    

    // inputs

    GtkWidget *materia = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(materia), 29);

    GtkWidget *titulo = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(titulo), 29);

    GtkWidget *nota = gtk_entry_new();

    // botao prazo | calendario popup
    GtkWidget *prazo;
    prazo=gtk_button_new_with_label("Prazo");

    GtkWidget *popprazo = gtk_popover_new(prazo);

    GtkWidget *calendar= gtk_calendar_new();

    // config dos input 
    gtk_entry_set_input_purpose(GTK_ENTRY(nota), GTK_INPUT_PURPOSE_NUMBER);

    // grid attach e center align
    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("materia:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), materia, 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("titulo:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), titulo, 0, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(criar), prazo, 1,1,1,1 );

    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("nota:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), nota, 0, 5, 1, 1);

    // botao cancela e cria
    GtkWidget *botcancel = gtk_button_new_with_label("cancelar");
    GtkWidget *botcriar = gtk_button_new_with_label("criar");

    gtk_grid_attach(GTK_GRID(criar), botcancel, 1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), botcriar, 2, 8, 1, 1);

    gtk_widget_set_halign(criar, GTK_ALIGN_CENTER );
    gtk_widget_set_valign(criar, GTK_ALIGN_CENTER );

    // config gpointer
    Global global;

    global.materia = materia;
    global.titulo = titulo;
    global.nota = nota;
    global.calendar = calendar;
    global.head = NULL;

    global.Datual = Datual;
    global.Matual = Matual;
    global.Aatual = Aatual;

    // config do grid ler ----0002----
    GtkWidget *ler;
    ler = gtk_grid_new(); 

    GtkWidget *botvoltar = gtk_button_new_with_label("voltar");
    gtk_grid_attach(GTK_GRID(ler), botvoltar, 0, 0, 1, 1);

    // config da stack  | pra alternar as telas inicio, criar, ler.
    GtkWidget *stack= gtk_stack_new();
    global.stack = stack;

    gtk_stack_add_named(GTK_STACK(stack), inicio , "inicio" );
    gtk_stack_add_named(GTK_STACK(stack), criar , "criar" );
    gtk_stack_add_named(GTK_STACK(stack), ler , "ler");

    // conecta botoes a suas respectivas fucoes

    // botoes voltar
    g_signal_connect(btc, "clicked", G_CALLBACK(trcriar), stack);

    g_signal_connect(botcancel, "clicked", G_CALLBACK(trinicio), stack);
    g_signal_connect(botvoltar, "clicked", G_CALLBACK(trinicio), stack);

    g_signal_connect(btl, "clicked", G_CALLBACK(trler), stack);

    // outras funçoes
    g_signal_connect(prazo, "clicked",G_CALLBACK(abrir_calendario), popprazo);

    g_signal_connect(botcriar, "clicked",G_CALLBACK(criar_atividade), &global);

    // funcao fechar janela
    g_signal_connect(window,"destroy", G_CALLBACK(gtk_main_quit),NULL); 

    // exibição
    gtk_container_add(GTK_CONTAINER(window), stack);

    gtk_container_add(GTK_CONTAINER(popprazo), calendar); 

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "inicio");
    gtk_widget_show_all(window);


    gtk_main();
    return 0;
}