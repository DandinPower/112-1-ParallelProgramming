### Create ssh public key / private key

- in local machine
    ```bash
    mkdir -p ~/.ssh
    ssh-keygen -t rsa
    cat ~/.ssh./id_rsa.pub
    ```

### Copy config into ssh folder

- copy the config into local machine ssh folder

- the config would look like 
    ```plaintext
    Host pp2
        HostName 192.168.202.2
        User 312581029
    Host pp10
        HostName 192.168.202.10
        User 312581029
    ```

- in local machine
    ```bash
    cp src/config ~/.ssh/
    ```

### Setup every Machine ssh config

- use ssh go to every machine and run the following scripts
    ```bash
    mkdir -p ~/.ssh
    export PUB_KEY="local machine ssh public key"
    echo $PUB_KEY >> ~/.ssh/authorized_keys
    ```