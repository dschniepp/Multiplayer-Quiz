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
    
}

void preparation_onWindowClosed(void){
    
}

void game_onAnswerClicked(int index){
    
}

void game_onWindowClosed(void){
    
} 