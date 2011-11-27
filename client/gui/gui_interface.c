#include "gui_interface.h"
#include "common/global.h"
#include "client/main.h"
#include "common/socket.h"

void preparation_onCatalogChanged(const char *newSelection){
    
    struct GB_CATALOG_CHANGE ca_ch;
    int ret=0;
    
    infoPrint("preparation_onCatalogChanged");
    
    
    /**Write CATALOG_CHANGE to server*/
        /**allocate memory to write*/
        
        ca_ch.catalog_msg = (char *)malloc(sizeof(newSelection)*sizeof(char));
        //strcpy(ca_ch.catalog_msg,*newSelection);
        infoPrint("send catalog_changed: %s", newSelection);
        prepare_message(&ca_ch.h, TYPE_CA_CH, strlen(newSelection));
        ret = write(get_socket(),&ca_ch.h,sizeof(ca_ch.h));
        ret = write(get_socket(),newSelection,strlen(newSelection));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }
        
    
    
}

void preparation_onStartClicked(const char *currentSelection){
    
    
    struct GB_START_GAME st_ga;
    int ret=0;
    
    
        /**allocate memory to write*/
    
        st_ga.catalog_msg = (char *)malloc(sizeof(currentSelection)*sizeof(char));
        
        //strcpy(ca_ch.catalog_msg,*newSelection);
        /**Make sure that string is Null terminated!!!*/
        
        /*strcpy(ca_ch.catalog_msg,currentSelection);
        int i=strlen(currentSelection);
        currentSelection[i]='\0';*/
    
        /**Write START_GAME to server*/
        
        infoPrint("send catalog_changed: %s", currentSelection);
        prepare_message(&st_ga.h, TYPE_ST_GA, strlen(currentSelection));
        ret = write(get_socket(),&st_ga.h,sizeof(st_ga.h));
        ret = write(get_socket(),currentSelection,strlen(currentSelection));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        } 
    
}

void preparation_onWindowClosed(void){
    guiShowMessageDialog("Sie haben das Spiel beendet!!!", 1);
}

void game_onAnswerClicked(int index){
    /**Send the answer to the server!*/
    
    struct GB_QUESTION_ANSWERED qu_an;
    int ret=0;
    
    qu_an.answer=(uint8_t)index;
    
    prepare_message(&qu_an, TYPE_QU_AN, 1);
        ret = write(get_socket(),&qu_an,1+sizeof(qu_an.h));
        test_return(ret);
        if (ret > 0) {
                infoPrint("Write to socket successful!");
        }     
}

void game_onWindowClosed(void){
    guiShowMessageDialog("Sie haben das Spiel beendet!!!", 1);
} 