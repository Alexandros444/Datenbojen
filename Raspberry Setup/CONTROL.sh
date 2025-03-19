#!/bin/bash

CONFIG_FILE="/etc/motion/motion.conf"
# CONFIG_FILE="motion_test.conf"
MOTION_LOG_LOC="/var/log/motion/motion.log"

KEY_VALUES=("width" "height" "framerate")

print_motion_info(){
    clear
    for ITEM in "${KEY_VALUES[@]}"; do
        # echo "Searching $KEY_VALUES in $CONFIG_FILE"
        grep "^$ITEM" $CONFIG_FILE
    done
    read -p "Press any key to return..." -n1 -s
}

motion_service_restart(){
    echo "Restarting Motion Service"
    sudo service motion restart
    sudo service motion status
    read -n1 -p "Print Log? [y,n]" doit
    case $doit in
        y|Y) cat $MOTION_LOG_LOC ;;
    esac
}

show_all_service_status(){
    systemctl --no-pager status motion
    echo ""
    systemctl --no-pager status icecast2
    echo ""
    systemctl --no-pager status ngrok
    read -p "Press any key to return..." -n1 -s
}

change_var_in_config() {
    local var_name=$1
    if [ -z "$var_name" ]; then
        echo "Error: variable name not provided! Ask Alex!"
        read -p "Press any key to return..." -n1 -s
        clear
        return
    fi

    NEW_VALUE=$(dialog --inputbox "Enter new $var_name :" 10 40 2>&1 >/dev/tty)
    if [ $? -ne 0 ] || [ -z "$NEW_VALUE" ]; then
        return
    fi

    clear

    # Update the resolution in the config file
    if grep -q "^$var_name " "$CONFIG_FILE"; then
        local old_var_value=$(sed -n "s/^$var_name //p" $CONFIG_FILE)
        sudo sed -i "s/^$var_name .*/$var_name $NEW_VALUE/" "$CONFIG_FILE"
        echo "$CONFIG_FILE $var_name $old_var_value -> $NEW_VALUE."
    else
        echo "Error did not find $var_name in config file! Ask Alex!"
    fi
    
    motion_service_restart

    read -p "Press any key to return..." -n1 -s
}

show_motion_menu() {
    while true; do
        MOTION_CHOICE=$(dialog --clear --title "Motion Options" --menu "Choose an option:" 15 50 3 \
        1 "Motion Image width" \
        2 "Motion Image height" \
        3 "Motion Framerate" \
        4 "Motion Info" \
        2>&1 >/dev/tty)

        if [ $? -ne 0 ]; then
            return
        fi

        clear
        case $MOTION_CHOICE in
            1)  change_var_in_config "width";;
            2)  change_var_in_config "height";;
            3)  change_var_in_config "framerate";;
            4)  print_motion_info;;
            *)  echo "Invalid choice!"
                read -p "Press any key to return..." -n1 -s;;
        esac
    done
}

# show_motion_menu
# clear
# exit 0

while true; do
    MAIN_CHOICE=$(dialog --clear --title "Main Menu" --menu "Choose an option:" 15 50 3 \
    1 "Motion Menu" \
    2 "Print Sercie Info" \
    2>&1 >/dev/tty)

    if [ $? -ne 0 ]; then
        echo "Exiting..."
        clear
        exit 0
    fi

    clear
    case $MAIN_CHOICE in
        1) show_motion_menu;;
        2) show_all_service_status;;
        *)
            echo "Invalid choice!"
            read -p "Press any key to return..." -n1 -s
            ;;
    esac
done