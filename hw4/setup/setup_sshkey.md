### Create ssh public key / private key

- in local machine
    ```bash
    mkdir -p ~/.ssh
    ssh-keygen -t rsa
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

- create hosts.txt
    ```plaintext
    pp2
    pp3
    pp4
    pp5
    pp6
    pp7
    pp8
    pp9
    pp10
    ```

- use ssh go to every machine and run the following scripts
    ```bash
    rm -f ~/.ssh/authorized_keys
    cat ~/.ssh/id_rsa.pub >> ~/.ssh/authorized_keys
    parallel-scp -A -h hosts.txt -r ~/.ssh ~
    ```