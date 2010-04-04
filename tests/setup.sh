chmod 664 data/*
rm -rf data
mkdir -p data

touch data/file
mkdir data/dir

touch data/file_access_denied
chmod 000 data/file_access_denied

touch data/file_read_only
chmod 444 data/file_read_only


mkdir data/dir_access_denied
chmod 000 data/dir_access_denied

mkdir data/dir_read_only
chmod 444 data/dir_read_only

echo '{"FromFile" : []}' > data/mru_ok.json

touch data/rename_me

touch data/rename_me_overwrite
touch data/rename_me_overwrite_renamed
