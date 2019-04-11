# blackstar
DevOps Software In C - This was written to execute devops tasks with more speed and flexibility. This code will allow for pushing out scripts or binaries written in any langauge to be executed on multiple servers. With the performance of C and threaded execution it should compete with the best of any devops tools. Allowing for code execution in any langauge should free up developers from having to learn a new langauge or being tied to only one tool for the job.

Install instructions are as follows:

	chmod +x configure
	./configure
	make
	make install

You may install the man page for this software on Debian distributions with the following:

	cp blackstar.8.gz /usr/share/man/man8/

This program can be removed like this:

	rm /usr/local/bin/blackstar

Some helpful information can be found with blackstar -h. Better documention from the man file can be found below:

The following are required for connecting to a single host:

	-l  Host: ip or domain. 
	-u  Username: should have sudo, but not enforced.
	-e  Commands to be executed. Use quotes if needed.

There also the following options for single hosts that are optional:

	-p  Port: if different that 22 for SSH.
	-w  Password for user. Special characters made require quotes.
	-d  Decrypt password file made with ccrypt.
	-o  Skip host checking

A brief usage can be displayed with -h. With the exceptions of -h and -o, all options require an argument. There is also the following option to use a file with multiple hosts defined:

	-f  Path to file of hosts.

The syntax of the file used to connect should use the following convention, but other options to SSH, ssphass or rsync can be used or added:

	sshpass -f passwdfile ssh -T user@example.com
	sshpass -f passwdfile ssh -T user@1.2.3.4
	sshpass -f passwdfile ssh -T user@localhost

The -f option will also require the -e option to issue a command to the hosts:

	blackstar -f file -e command

If a password is set in the SSHPASS environment variable and it is exported, the following syntax may be used in file inside a file of hosts instead:

	sshpass -e ssh -T user@localhost

Keep in mind that the SSHPASS variable may only hold one password. So you can use other variables for different passwords that are specified like this:

	sshpass -p $CHEESE ssh -T user@localhost
	
The literal password can be used in place of the variable in the commands above. Though for security it is better to just use variables, encrypted password files or ssh keys. If ssh keys are used for logins, lines with only the following can be used a file of hosts:

	ssh -T user@example.com

This program will easily open .cpt files made with ccrypt using its -d option: 

	blackstar -f filename -e date -d passwd.cpt

This will prompt the user for the password to this file and after commands are executed on hosts you will be asked to enter a new password to re-encrypt the decrypted file to keep it safe.

Scripts and even binaries can be pushed out to hosts as well. This will require rsync synax like below in a file with hosts:

	sshpass -e rsync -av /path/to/script user@localhost:/tmp/

Then you would run the following to make the file executable and run it:

	blackstar -f file -e "&& chmod +x /tmp/script && /tmp/script"

Escaping will rarely be needed, but certain situation may require escaping or using quotes around the arugument(s) for -e or passwords with special characters.

To issue commands with sudo, lines like the following to prevent being prompted for a password:
 
	echo $SSHPASS | sshpass -e ssh -T ph33r@localhost

Then you can use 'sudo -S command' with the -e option and you will not be prompted for the password in $SSHPASS.

SEE ALSO
sshpass(8), ccrypt(5), blackstar.debian(8) 

AUTHOR:
Jon Cox (cloneozone@gmail.com)
Copyright (c) 2019, Jon Cox
