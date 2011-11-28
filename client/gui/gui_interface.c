//#include "gui_interface.h"
//#include "client/main.h"
//#include "common/socket.h"
#include "client/common/client_global.h"


void preparation_onCatalogChanged(const char *newSelection){
    
    struct GB_CATALOG_CHANGE ca_ch;
    int ret=0;    
    
    /**allocate memory to write*/
        
    ca_ch.catalog_msg = (char *)malloc(sizeof(newSelection)*sizeof(char));
    
    /**Write CATALOG_CHANGE to server*/
    
    infoPrint("send catalog_changed: %s", newSelection);
    prepare_message(&ca_ch.h, TYPE_CA_CH, strlen(newSelection));
    ret = write(get_socket(),&ca_ch.h,sizeof(ca_ch.h));
    test_socketOnErrors(ret);
    ret = write(get_socket(),newSelection,strlen(newSelection));
    test_socketOnErrors(ret);
    infoPrint("Write to socket successful!");
    free(ca_ch.catalog_msg);      
}

void preparation_onStartClicked(const char *currentSelection){
    
    struct GB_START_GAME st_ga;
    int ret=0;
    
    /**allocate memory to write*/
    
    st_ga.catalog_msg = (char *)malloc(sizeof(currentSelection)*sizeof(char));
    
    /**Write START_GAME to server*/
        
    infoPrint("send catalog_changed: %s", currentSelection);
    prepare_message(&st_ga.h, TYPE_ST_GA, strlen(currentSelection));
    ret = write(get_socket(),&st_ga.h,sizeof(st_ga.h));
    test_socketOnErrors(ret);
    ret = write(get_socket(),currentSelection,strlen(currentSelection));
    test_socketOnErrors(ret);
    infoPrint("Write to socket successful!");
    free(st_ga.catalog_msg);        
}

void preparation_onWindowClosed(void){
    guiShowMessageDialog("Sie haben das Spiel beendet!!!", 1);
}

void game_onAnswerClicked(int index){
    
    struct GB_QUESTION_ANSWERED qu_an;
    int ret=0;
    
    /**Write the users answer to the server!*/
    
    qu_an.answer=(uint8_t)index;
    
    prepare_message(&qu_an, TYPE_QU_AN, 1);
    ret = write(get_socket(),&qu_an,1+sizeof(qu_an.h));
    test_socketOnErrors(ret);
    infoPrint("Write to socket successful!");  
}

void game_onWindowClosed(void){
    guiShowMessageDialog("Sie haben das Spiel beendet!!!", 1);
} 