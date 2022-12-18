#!/bin/bash

# Auto-updated by CMake with actual install path
SRSLTE_INSTALL_DIR="/usr/local/share/srslte"

# check if install mode has been provided
if ([ ! $1 ])
then
  echo "Please call script with either user or service as first parameter."
  echo ""
  echo "E.g. ./srslte_install_configs.sh user"
  echo " .. to install all config files to $HOME/.config/srslte"
  echo ""
  echo "E.g. ./srslte_install_configs.sh service"
  echo " .. to install all config files to /etc/srslte"
  exit
fi

if [ "$1" == "user" ]
then
  dest_folder="$HOME/.config/srslte"
elif [ "$1" == "service" ]
then
  dest_folder="/etc/srslte"
else
  echo "Please call script with either user or service as first parameter."
  exit
fi

if [ "$2" == "--force" ]
then
  echo "Force overwrite called. Overwritting any existing configuration files."
  read -e -p "Is this OK? [y/n]: " choice
  [[ "$choice" == [Yy]* ]] && force_install="true" || exit
elif [ "$2" == "--force-no-db" ]
then
  echo "Force overwrite called. Overwritting all existing configuration files, except ue_db.csv."
  read -e -p "Is this OK? [y/n]: " choice
  if [[ "$choice" == [Yy]* ]] ; then force_install="true" && no_db="true"; else exit; fi
elif [ "$2" == "" ]
then
  : #NOOP
else
  echo "Invalid command line option provided."
  exit 
fi


install_file(){
  source_path="$SRSLTE_INSTALL_DIR/$1"
  dest_path=$(echo "$dest_folder/$1" | sed 's/\.[^.]*$//') # Strip .example from filename

  # Check if config file already exists in location
  if [ -f  $dest_path ]; then
    # If --force option is selected, overwrite file 
    if [ "$force_install" != "true" ] && [ "$no_db" != "true" ] ; then
      echo " - $dest_path already exists. Skipping it."
      return
    fi
    # If --force-no-db option is selected, overwrite file unless it ends in ".csv"
    if [ "$force_install" == "true" ] && [ "$no_db" == "true" ] && [[ $dest_path =~ \.csv$ ]]; then
      echo " - $dest_path already exists. Skipping it."
      return
    fi
  fi

  # Check if config file exists in source location
  if [ -f $source_path ]; then
    echo " - Installing $1 in $dest_path"
    cp $source_path $dest_path
    
    # Set file ownership to user calling sudo
    if [ $SUDO_USER ]; then
      user=$SUDO_USER
      chown $user:$user $dest_path
    fi
  else
    echo " - $source_path doesn't exists. Skipping it."
  fi

  return
}

# Install all srsLTE config files
echo "Installing srsLTE configuration files:"

# Make sure the target directory exists
if [ ! -d "$dest_folder" ]; then
  echo " - Creating srsLTE config folder $dest_folder"
  mkdir -p $dest_folder
  if [ $? -ne 0 ]; then
    exit
  fi
fi


install_file "ue.conf.example"
install_file "enb.conf.example"
install_file "sib.conf.example"
install_file "rr.conf.example"
install_file "drb.conf.example"
install_file "epc.conf.example"
install_file "mbms.conf.example"
install_file "user_db.csv.example"

echo "Done."
