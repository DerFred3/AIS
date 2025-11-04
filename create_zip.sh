#!/bin/bash

# parse parameters
REPLACE=0
DRY_RUN=0
SILENT=0

# basic usage of script
usage() { 
        echo "Usage: $0 [-r] [-n] [-s]" 1>&2;
        echo ""
        echo " where -r stands for replacing archives"
        echo "   and -n stands for explicitly not executing *any* changes (dry run)"
        echo "   and -s stands for silent, which suppresses most print outs"

        exit 1
}

# utility for logging
log() {
        if [ ${SILENT} -eq 0 ]; then
                echo "$1"
        fi
}

# read call options
while getopts ":rns" o; do
        case "${o}" in
                r)
                        REPLACE=1
                        ;;
                n)
                        DRY_RUN=1
                        ;;
                s)
                        SILENT=1
                        ;;
                *)
                        usage
                        ;;
        esac
done

config_name=".zip_config"
lines_read=0
zip_name=""
full_name=""

# read name-config file
while read line; do
        lines_read=$(($lines_read+1))
        if [ ${lines_read} -eq 1 ]; then
                # ignore first line
                continue
        elif [ ${lines_read} -eq 2 ]; then
                # split line and fill zip_name in correct format
                full_name=${line}
                for name in $line; do
                        zip_name="${zip_name}_${name}"
                done
        else
                # only accept second line
                break
        fi
done < "${PWD}/${config_name}"

# check if actual content was read
if [ -z ${zip_name} ]; then
        echo "Could not find a name within file <${PWD}/${config_name}>" 1>&2; exit 1;
fi

log "Found name <${full_name}> in file <${PWD}/${config_name}>"

log "Processing folders.."
log ""

# continue iterating over task folders
for dir in [0-9][0-9]* ; do
        full_path="${PWD}/${dir}"

        log "  ${full_path}:"

        files_cfg="${full_path}/${config_name}"

        # check if zip-config exists within task folder
        if ! test -f ${files_cfg}; then
                log "    - No config file found in here, continuing with next folder.."
                log ""
                continue
        fi

        lines_read=0

        # read zip-config of task folder
        while read line; do
                lines_read=$(($lines_read+1))
                if [ ${lines_read} -eq 1 ]; then
                        # ignore first line
                        continue
                elif [ ${lines_read} -eq 2 ]; then
                        # trim leading 0s
                        if [ ${dir:0:1} -eq 0 ]; then
                                zip_name_full="a${dir:1:1}${zip_name}.zip"
                        else
                                zip_name_full="a${dir:0:2}${zip_name}.zip"
                        fi
                        
                        log "    - Zip filename determined: <${zip_name_full}>"
                        log "    - To-be-zipped files: <${line}>"

                        # Only create zip, if not a dry run and file does not exist, or REPLACE flag is set
                        if [ ${DRY_RUN} -eq 0 ] && [ ! -f ${full_path}/${zip_name_full} ] || [ ${REPLACE} -eq 1 ]; then
                                log "    - Trying to execute: zip ${full_path}/${zip_name_full} ${full_path}/${line}" 

                                # actual command execution using zip
                                zip ${full_path}/${zip_name_full} ${full_path}/${line}
                        else
                                if [ ${DRY_RUN} -eq 1 ]; then
                                        log "    - -n flag was set; no zip file was created!"
                                fi

                                if [ ${REPLACE} -eq 0 ] && [ -f ${full_path}/${zip_name_full} ] && [ ${DRY_RUN} -eq 0 ]; then
                                        log "    - Replace flag not set; zip file was *not* overriden!"
                                fi
                        fi
                else
                        # only accept second line
                        break
                fi
        done < ${files_cfg}
        log ""
done
