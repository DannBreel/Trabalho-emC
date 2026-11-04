#include <gtk/gtk.h>

// struct pra salvar as atvidades -- lista simplesmente encadeada --
typedef struct{
    // info principal
    char titulo[30];
    char materia[30];
    char prazo[10];
    int nota;

    // no para o proximo
    tarefa* proximo;

}tarefa;


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

    // ----- codigo comeca aqui ----- \\ 

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

    GtkWidget *materia = gtk_entry_new();
    GtkWidget *titulo = gtk_entry_new();

    GtkWidget *prazo = gtk_entry_new();
    GtkWidget *nota = gtk_entry_new();

    GtkWidget *calendario= gtk_calendar_new();

    guint dia,mes.ano;

    
    // config dos input 
    gtk_entry_set_max_length(GTK_ENTRY(prazo), 10);
    gtk_entry_set_input_purpose(GTK_ENTRY(prazo), GTK_INPUT_PURPOSE_DIGITS);
    gtk_entry_set_input_purpose(GTK_ENTRY(nota), GTK_INPUT_PURPOSE_NUMBER);

    // grid attach e center align
    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("materia:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), materia, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("titulo:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), titulo, 0, 3, 1, 1);
    //gtk_grid_attach(GTK_GRID(criar), gtk_label_new("prazo(dd/mm/aaaa):"), 0, 4, 1, 1);
    //gtk_grid_attach(GTK_GRID(criar), prazo, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), gtk_label_new("nota:"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), nota, 0, 7, 1, 1);

    // botao cancela e cria
    GtkWidget *botcancel = gtk_button_new_with_label("cancelar");
    GtkWidget *botcriar = gtk_button_new_with_label("criar");

    gtk_grid_attach(GTK_GRID(criar), botcancel, 1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), botcriar, 2, 8, 1, 1);

    gtk_widget_set_halign(criar, GTK_ALIGN_CENTER );
    gtk_widget_set_valign(criar, GTK_ALIGN_CENTER );

    // config do grid ler ----0001----
    GtkWidget *ler;
    ler = gtk_grid_new(); 

    GtkWidget *botvoltar = gtk_button_new_with_label("voltar");
    gtk_grid_attach(GTK_GRID(ler), botvoltar, 0, 0, 1, 1);

    // config da stack  | pra alternar as telas inicio, criar, ler.
    GtkWidget *stack= gtk_stack_new();
    
    gtk_stack_add_named(GTK_STACK(stack), inicio , "inicio" );
    gtk_stack_add_named(GTK_STACK(stack), criar , "criar" );
    gtk_stack_add_named(GTK_STACK(stack), ler , "ler");

    // conecta botoes a suas respectivas fucoes
    g_signal_connect(btc, "clicked", G_CALLBACK(trcriar), stack);

    g_signal_connect(botcancel, "clicked", G_CALLBACK(trinicio), stack);
    g_signal_connect(botvoltar, "clicked", G_CALLBACK(trinicio), stack);

    g_signal_connect(btl, "clicked", G_CALLBACK(trler), stack);

    // funcao fechar janela
    g_signal_connect(window,"destroy", G_CALLBACK(gtk_main_quit),NULL); 

    // exibição
    gtk_container_add(GTK_CONTAINER(window), stack);

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "inicio");
    gtk_widget_show_all(window);


    gtk_main();
    return 0;
}