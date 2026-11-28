#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <stdbool.h> // pra comilar no linux :p

// struct pra salvar as atvidades -- lista simplesmente encadeada --
typedef struct Tarefa{
    // info principal
    char titulo[30];
    char materia[30];

    char data[11];
    int data_sist;

    char prazo[11];
    int prazo_sist;

    int nota;

    // no para o proximo
    struct Tarefa* proximo;
}Tarefa;

// variaveis para deixar global
typedef struct Global{

    GtkWidget *materia;
    GtkWidget *titulo;
    GtkWidget *nota;
    GtkWidget *calendar;

    GtkWidget *stack;
    GtkWidget *window;
    GtkWidget *listbox;

    bool editando;
    Tarefa *subedit;

    int Datual;
    int Matual;
    int Aatual;

    Tarefa *head;

} Global;

// pra avisar que a função existe
void atualizar_lista(Global *global);
void save_files(Global *global);
void load_files(Global *global);

// funçoes outras
void popup_msg(GtkWidget *janela, const char *mensagem) {

    GtkWidget *msg = gtk_message_dialog_new(

        GTK_WINDOW(janela),            
        GTK_DIALOG_MODAL,            
        GTK_MESSAGE_WARNING,        
        GTK_BUTTONS_OK,                
        "%s", mensagem                
    );

    gtk_dialog_run(GTK_DIALOG(msg));   
    gtk_widget_destroy(msg);           
}

void abrir_calendario(GtkWidget *botao, gpointer dados) {
    GtkWidget *popover = GTK_WIDGET(dados);

    gtk_widget_show_all(popover);
    gtk_popover_popup(GTK_POPOVER(popover));
}

void limpar_campos(Global *global) { // dados precisa ser &global

    gtk_entry_set_text(GTK_ENTRY(global->materia), "");
    gtk_entry_set_text(GTK_ENTRY(global->titulo), "");
    gtk_entry_set_text(GTK_ENTRY(global->nota), "");

    gtk_calendar_select_month(GTK_CALENDAR(global->calendar),(global->Matual-1),global->Aatual);
    gtk_calendar_select_day(GTK_CALENDAR(global->calendar),global->Datual);

}

void excluir(GtkWidget *botao, gpointer dados){
    Global *global = g_object_get_data(G_OBJECT(botao), "global");
    Tarefa *alvo;

    if(global->subedit && global->editando == true){
        alvo = global->subedit;
        global->subedit = NULL;
        global->editando = FALSE;
    }
    else{
    alvo = dados;
    }

    Tarefa **pp = &(global->head);  // pp == &(global.head) 
                                   // *pp == global->head->proximo
    while (*pp && *pp != alvo){    // global->head->proximo != alvo
         pp = &(*pp)->proximo;    //== pp = &(global->head->proximo)->proximo | reapontando para o ponteiro denovo
    }
    if (*pp) {
        *pp = alvo->proximo;      // global->head->proximo = alvo->proximo |
        free(alvo);
    }
    
    atualizar_lista(global);  

}

void editar(GtkWidget *botao, gpointer dados){

    Tarefa *alvo = dados;
    Global *global = g_object_get_data(G_OBJECT(botao), "global");

    global->editando = TRUE;
    global->subedit = alvo;

    GtkStack *stack = GTK_STACK(global->stack);
    gtk_stack_set_visible_child_name(stack, "criar");

    gtk_entry_set_text(GTK_ENTRY(global->titulo), alvo->titulo);
    gtk_entry_set_text(GTK_ENTRY(global->materia), alvo->materia);

    char nota[30];
    snprintf(nota, sizeof(nota), "%d", alvo->nota);
    gtk_entry_set_text(GTK_ENTRY(global->nota), nota);

    int ano = (alvo->prazo_sist)/10000;          // ano == aaaammdd = aaaa,mmdd = aaaa
    int mes = ((alvo->prazo_sist)/100);          // mes == aaaamm,dd = aaaamm
    int dia = ((alvo->prazo_sist)-(mes*100));    // dia == aaaammdd - aaaamm00 = dd
    mes=(mes-(ano*100))-1;                       // mes == aaaamm - aaaa00 = mm

    gtk_calendar_select_month(GTK_CALENDAR(global->calendar),mes,ano);
    gtk_calendar_select_day(GTK_CALENDAR(global->calendar),dia);

}

void atualizar_lista(Global *global) {

    // apaga as linhas que já estavam lá
    GList *filhos = gtk_container_get_children(GTK_CONTAINER(global->listbox));

    for (GList *i = filhos; i != NULL; i = i->next){
        gtk_widget_destroy(GTK_WIDGET(i->data));
    }

    g_list_free(filhos);

    //  percorre a lista encadeada e cria uma linha por tarefa
    for (Tarefa *t = global->head; t != NULL; t = t->proximo) {
        
        char texto[150];
        snprintf(texto, sizeof(texto), "%s - %s\nData: %s Prazo: %s | Nota: %d",
                 t->materia, t->titulo, t->data, t->prazo, t->nota);

        GtkWidget *label = gtk_label_new(texto);
        gtk_widget_set_name(label, "texto-atividade");
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
        gtk_label_set_line_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD_CHAR);
        gtk_label_set_xalign(GTK_LABEL(label), 0);
        gtk_widget_set_hexpand(label, TRUE);
        gtk_widget_set_halign(label, GTK_ALIGN_FILL);

        GtkWidget *bt_editar  = gtk_button_new_with_label("editar");
        GtkWidget *bt_excluir = gtk_button_new_with_label("excluir");
        gtk_widget_set_name(bt_editar, "botao-editar");
        gtk_widget_set_name(bt_excluir, "botao-excluir");
        gtk_widget_set_valign(bt_editar,  GTK_ALIGN_CENTER);
        gtk_widget_set_valign(bt_excluir, GTK_ALIGN_CENTER);

        // o botao vai levar o global e a tarefa vai no dados
        g_object_set_data(G_OBJECT(bt_editar),  "global", global);
        g_object_set_data(G_OBJECT(bt_excluir), "global", global);

        g_signal_connect(bt_editar,  "clicked", G_CALLBACK(editar),  t);
        g_signal_connect(bt_excluir, "clicked", G_CALLBACK(excluir), t);

        GtkWidget *linha = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 14);
        gtk_widget_set_margin_start(linha, 12);
        gtk_widget_set_margin_end(linha, 12);
        gtk_widget_set_margin_top(linha, 8);
        gtk_widget_set_margin_bottom(linha, 8);

        gtk_box_pack_start(GTK_BOX(linha), label, TRUE, TRUE, 0);   
        gtk_box_pack_end(GTK_BOX(linha), bt_excluir, FALSE, FALSE, 0); 
        gtk_box_pack_end(GTK_BOX(linha), bt_editar,  FALSE, FALSE, 0); 

        gtk_list_box_insert(GTK_LIST_BOX(global->listbox), linha, -1);
    }

    //  widgets criados depois da janela aberta precisam disso para aparecer
    gtk_widget_show_all(global->listbox);
}

void criar_atividade(GtkWidget *botao, gpointer dados) {

    Global *global = dados;

    Tarefa *novo = malloc(sizeof(Tarefa));
    if (novo == NULL) return;

    guint ano, mes, dia;
    gtk_calendar_get_date(GTK_CALENDAR(global->calendar), &ano, &mes, &dia);

    snprintf(novo->prazo, sizeof(novo->prazo), "%02u/%02u/%04u", dia, mes + 1, ano); // mes +1 porque o sistema le como 11
    novo->prazo_sist = ano*10000 + (mes+1)*100 + dia; 

    if(global->subedit && global->editando == TRUE){
        strcpy(novo->data , global->subedit->data);
        novo->data_sist = global->subedit->data_sist;
    }
    else{
    snprintf(novo->data, sizeof(novo->data), "%02u/%02u/%04u", global->Datual, global->Matual, global->Aatual); 
    novo->data_sist = global->Aatual*10000 + global->Matual*100 + global->Datual; 
    }

    // exclui o novo caso o prazo seja antes da data atual
    if((novo->prazo_sist)<(novo->data_sist)){
        free(novo);
        popup_msg(global->window, "Prazo invalido");
        return;
    }

    const char *mat = gtk_entry_get_text(
        GTK_ENTRY(global->materia)
    );

    const char *tit = gtk_entry_get_text(
        GTK_ENTRY(global->titulo)
    );

    const char *nota = gtk_entry_get_text(
        GTK_ENTRY(global->nota)
    );

    // gambiarra do caraio em, e redundante >:(  -d
    for (int i = 0; nota[i] != '\0'; i++) {
    if (!g_ascii_isdigit(nota[i])) {
        popup_msg(global->window, "Digite apenas numeros na nota!");
        free(novo); // alguem esqueceu disso. -d
        return;
        }
    }

    snprintf(novo->materia, sizeof(novo->materia), "%s", mat);
    snprintf(novo->titulo, sizeof(novo->titulo), "%s", tit);

    novo->nota = atoi(nota); // transforma a nota em int

    // empurra pra frente
    novo->proximo = global->head;
    global->head = novo;

    if(global->subedit && global->editando == true){
        excluir(botao, dados);
    }

    // limpa os campos e volta pro inicio
    limpar_campos(dados);

    GtkStack *stack = GTK_STACK(global->stack);
    gtk_stack_set_visible_child_name(stack, "inicio");
}

void save_files(Global *global){

    FILE *f = fopen("tarefas.dat","wb");
    if(f==NULL) return;

    for(Tarefa *tar = global->head; tar!=NULL ; tar=tar->proximo){
        fwrite(tar,sizeof(Tarefa),1,f);
    }

    fclose(f);

}

void load_files(Global *global){

    FILE *f = fopen("tarefas.dat","rb");
    if(f==NULL) return;

    Tarefa **fim=&global->head;

    while(TRUE){
        Tarefa *novo=malloc(sizeof(Tarefa));
        if(novo==NULL)break;

        if(fread(novo,sizeof(Tarefa),1,f)!=1){
            free(novo);
            break;
        }
        novo->proximo=NULL;
        *fim = novo;
        fim = &novo->proximo;
    }
    fclose(f);
}

// muda a tela para a de criaçao, inicio e ler respectivamente.

void trcriar(GtkWidget *botao, gpointer dados) {
    GtkStack *stack = GTK_STACK(dados);

    gtk_stack_set_visible_child_name(stack, "criar");
}

void trinicio(GtkWidget *botao, gpointer dados) {
    Global *global = dados;
    GtkStack *stack = GTK_STACK(global->stack);

    limpar_campos(global);

    if(global->editando == TRUE && global->subedit){
        global->editando = FALSE;
        global->subedit = NULL;
    }

    gtk_stack_set_visible_child_name(stack, "inicio");
}

void trler(GtkWidget *botao, gpointer dados) {
    Global *global = dados;
    GtkStack *stack = GTK_STACK(global->stack);

    atualizar_lista(dados);
    gtk_stack_set_visible_child_name(stack, "ler");
}

int main(int argc, char **argv) {
    Global global;
    
    gtk_init(&argc, &argv);
    // Carregar arquivo CSS
    GtkCssProvider *css = gtk_css_provider_new();
    GError *erro = NULL;

    gboolean carregou = gtk_css_provider_load_from_path(
        css,
        "agenda.css",
        &erro
    );

    if (carregou) { // que? -D
        printf("CSS carregado com sucesso!\n");
    } else {
    printf("Erro ao carregar CSS: %s\n", erro->message);
    g_error_free(erro);
}

gtk_style_context_add_provider_for_screen(
    gdk_screen_get_default(),
    GTK_STYLE_PROVIDER(css),
    GTK_STYLE_PROVIDER_PRIORITY_USER
);

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
    // Titulo e subtitulo
    GtkWidget *titulo_inicio = gtk_label_new("Minha Agenda");
    GtkWidget *subtitulo = gtk_label_new("Organize suas atividades e prazos");
    gtk_widget_set_name(titulo_inicio, "titulo-inicio");
    gtk_widget_set_name(subtitulo, "subtitulo-inicio");
    gtk_grid_attach(GTK_GRID(inicio), titulo_inicio, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(inicio), subtitulo, 0, 1, 2, 1);

    // config butoes
    GtkWidget *btc;
    GtkWidget *btl;
    btc=gtk_button_new_with_label("Criar");
    btl=gtk_button_new_with_label("Ler");
    gtk_widget_set_name(btc, "botao-criar");
    gtk_widget_set_name(btl, "botao-ler");

    gtk_grid_set_row_spacing(GTK_GRID(inicio), 15);
    gtk_grid_set_column_spacing(GTK_GRID(inicio), 15);
    gtk_widget_set_halign(inicio, GTK_ALIGN_CENTER );
    gtk_widget_set_valign(inicio, GTK_ALIGN_CENTER );

    // config Botoes e grid
    gtk_grid_attach(GTK_GRID(inicio), btc, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(inicio), btl, 0, 3, 2, 1);

    // config do grid Criar ----0001----
    GtkWidget *criar;
    criar = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(criar), 10);
    gtk_grid_set_column_spacing(GTK_GRID(criar), 12);

    // inputs

    GtkWidget *materia = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(materia), 29);

    GtkWidget *titulo = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(titulo), 29);

    GtkWidget *nota = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(nota), 6);
    gtk_entry_set_input_purpose(GTK_ENTRY(nota), GTK_INPUT_PURPOSE_NUMBER);
    
    gtk_widget_set_name(materia, "campo-materia");
    gtk_widget_set_name(titulo, "campo-titulo");
    gtk_widget_set_name(nota, "campo-nota");

    // botao prazo | calendario popup
    GtkWidget *prazo;
    prazo=gtk_button_new_with_label("Prazo");

    GtkWidget *popprazo = gtk_popover_new(prazo);

    GtkWidget *calendar= gtk_calendar_new();

    // config dos input 
    gtk_entry_set_input_purpose(GTK_ENTRY(nota), GTK_INPUT_PURPOSE_NUMBER);

    // grid attach e center align

    GtkWidget *label_materia = gtk_label_new("Matéria");
    GtkWidget *label_titulo = gtk_label_new("Título");
    GtkWidget *label_nota = gtk_label_new("Nota");

    gtk_widget_set_name(label_materia, "label-formulario");
    gtk_widget_set_name(label_titulo, "label-formulario");
    gtk_widget_set_name(label_nota, "label-formulario");

    gtk_grid_attach(GTK_GRID(criar), label_materia, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), materia, 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(criar), label_titulo, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), titulo, 0, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(criar), prazo, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(criar), label_nota, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), nota, 0, 5, 1, 1);

    // botao cancela e cria
    GtkWidget *botcancel = gtk_button_new_with_label("Cancelar");
    GtkWidget *botcriar = gtk_button_new_with_label("Pronto");
    g_object_set_data(G_OBJECT(botcriar),"global",&global);
    gtk_widget_set_name(botcancel, "botao-cancelar");
    gtk_widget_set_name(botcriar, "botao-pronto");
    gtk_widget_set_name(prazo, "botao-prazo");

    gtk_grid_attach(GTK_GRID(criar), botcancel, 1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(criar), botcriar, 2, 6, 1, 1);

    gtk_widget_set_halign(criar, GTK_ALIGN_CENTER );
    gtk_widget_set_valign(criar, GTK_ALIGN_CENTER );

    // config da struct global

    global.editando = FALSE;
    global.subedit = NULL;

    global.window = window;

    global.materia = materia;
    global.titulo = titulo;
    global.nota = nota;
    global.calendar = calendar;
    global.head = NULL;

    global.Datual = Datual;
    global.Matual = Matual;
    global.Aatual = Aatual;

    load_files(&global);

    // config do grid ler ----0002----
    GtkWidget *ler;
    ler = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(ler), 15);
    gtk_grid_set_column_spacing(GTK_GRID(ler), 15);

    gtk_widget_set_margin_top(ler, 20);
    gtk_widget_set_margin_bottom(ler, 20);
    gtk_widget_set_margin_start(ler, 20);
    gtk_widget_set_margin_end(ler, 20);

    // sistema de scroll
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);

    // config da list box da visualização
    GtkWidget *listbox = gtk_list_box_new();

    gtk_container_add(GTK_CONTAINER(scroll), listbox);
    gtk_grid_attach(GTK_GRID(ler), scroll, 0, 1, 1, 1);

    global.listbox = listbox;

    GtkWidget *botvoltar = gtk_button_new_with_label("Voltar");
    gtk_widget_set_name(botvoltar, "botao-voltar");
    gtk_widget_set_name(listbox, "lista-atividades");
    gtk_widget_set_name(scroll, "area-rolagem");
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

    g_signal_connect(botcancel, "clicked", G_CALLBACK(trinicio), &global);
    g_signal_connect(botvoltar, "clicked", G_CALLBACK(trinicio), &global);

    g_signal_connect(btl, "clicked", G_CALLBACK(trler), &global);

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
    
    // codigo que roda só depois que o app fecha
    save_files(&global);

    Tarefa *del=global.head;
    while(global.head!=NULL){
        global.head=global.head->proximo;
        free(del);
        del=global.head;
    }

    return 0;
}