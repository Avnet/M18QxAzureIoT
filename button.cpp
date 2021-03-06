/**
* copyright (c) 2018, James Flynn
* SPDX-License-Identifier: MIT
*/

/**
*   @file   button.cpp
*   @brief  the member functions assoicated with the button class. the button_handler_thread runs continuously handling 
*           button press/release events as indicated by a button interrupt.
*
*   @author James Flynn
*
*   @date   1-Oct-2018
*/

#include "button.hpp"

void *Button::button_handler_thread(void *obj)
{
    Button *data = static_cast<Button *>(obj);
    gpio_level_t   pin_state;
    struct timespec keypress_duration;

    pthread_mutex_lock(&data->button_mutex);
    while( data->button_active ) {
        pthread_cond_wait(&data->button_wait, &data->button_mutex);
        if( !data->button_active )
            continue;
        pin_state = data->pin_state;
        if (!pin_state) {
            data->button_press = true;
            clock_gettime(CLOCK_MONOTONIC, &data->key_press);
            if( data->bp_cb )
                data->bp_cb();
            }
        else{
            data->button_press = false;
            clock_gettime(CLOCK_MONOTONIC, &data->key_release);
            if ((data->key_release.tv_nsec - data->key_press.tv_nsec)<0) 
                keypress_duration.tv_sec = data->key_release.tv_sec - data->key_press.tv_sec-1;
            else 
                keypress_duration.tv_sec = data->key_release.tv_sec - data->key_press.tv_sec;
       
            if( data->br_cb )
                data->br_cb(keypress_duration.tv_sec);
            }
        } //never return
    pthread_exit(0);
}

