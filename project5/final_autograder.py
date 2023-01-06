import os
import time
import hashlib
from multiprocessing import Process
from os.path import exists
import socket
import sys


def check_hash(file_name):
	os.chdir(root_dir + "/Assignment 5")
	local_path = "client_domain/Local Directory/"
	remote_path = "server_domain/Remote Directory/"

	try:
		file_1 = local_path + file_name
		os.system("chmod +rwx client_domain/Local\ Directory/" + file_name + " &>/dev/null")
		md5_1 = hashlib.md5()
		file_1_hash = open(file_1, 'rb')
		file_1_bytes = file_1_hash.read()
		md5_1.update(file_1_bytes)
		file_1_hash.close()

		file_2 = remote_path + file_name
		os.system("chmod +rwx server_domain/Remote\ Directory/" + file_name + " &>/dev/null")
		md5_2 = hashlib.md5()
		file_2_hash = open(file_2, 'rb')
		file_2_bytes = file_2_hash.read()
		md5_2.update(file_2_bytes)
		file_2_hash.close()

		if md5_1.hexdigest() == md5_2.hexdigest():
			print("SUCCESS: hash match for", file_name)
		else:
			print("FAILURE: hash did not match for", file_name)
			# print("hash of", file_1, "=", md5_1.hexdigest())
			# print("hash of", file_2, "=", md5_2.hexdigest())
		can_download = True
	except FileNotFoundError:
		print("FAILURE: file", file_name, "could not be found in one of the directories.")
		can_download = False
	return can_download


def initialize_directories():
	os.system("rm -rf client_domain/Local\ Directory")
	os.system("rm -rf server_domain/Remote\ Directory")
	os.system("mkdir client_domain/Local\ Directory")
	os.system("mkdir server_domain/Remote\ Directory")


def terminate_processes(client_pid, server_pid):
	os.system("kill -2 " + str(server_pid))
	os.system("kill -2 " + str(client_pid))

	# print("terminating client process ID:", client_pid)
	os.system("kill -9 " + str(client_pid))

	# print("terminating server process ID:", server_pid)
	os.system("kill -9 " + str(server_pid))
	os.system("pkill -9 -f client &>/dev/null")
	os.system("pkill -9 -f server &>/dev/null")


def start_server(server_ip):
	initialize_directories()
	os.chdir(root_dir + "/Assignment 5/server_domain")
	os.system("./server " + server_ip + " &>/dev/null")
	# print("---------- server existed successfully ----------")


def start_download_testcase(server_ip):
	os.chdir(root_dir)
	os.system("cp download_testcase/file2.jpeg Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp download_testcase/file1.txt Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp download_testcase/test_commands.txt Assignment\ 5/client_domain")
	os.chdir("Assignment 5/client_domain")
	os.system("./client test_commands.txt " + server_ip)
	# print("\n---------- client existed successfully ----------")


def start_upload_testcase(server_ip):
	os.chdir(root_dir)
	os.system("cp upload_testcase/file2.jpeg  Assignment\ 5/client_domain/Local\ Directory")
	os.system("cp upload_testcase/file1.txt Assignment\ 5/client_domain/Local\ Directory")
	os.system("cp upload_testcase/test_commands.txt Assignment\ 5/client_domain")
	os.chdir("Assignment 5/client_domain")
	os.system("./client test_commands.txt " + server_ip)
	# print("\n---------- client existed successfully ----------")


def start_delete_testcase(server_ip):
	os.chdir(root_dir)
	os.system("cp delete_testcase/file1.txt Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp delete_testcase/file3.jpeg Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp delete_testcase/file2.txt Assignment\ 5/client_domain/Local\ Directory")
	os.system("cp delete_testcase/test_commands.txt Assignment\ 5/client_domain")

	os.chdir("Assignment 5/client_domain")
	os.system("./client test_commands.txt " + server_ip)
	# print("\n---------- client existed successfully ----------")


def start_append_testcase(server_ip):
	os.chdir(root_dir)
	os.system("cp append_testcase/file1.txt Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp append_testcase/file2.txt  Assignment\ 5/server_domain/Remote\ Directory")
	os.system("cp append_testcase/test_commands.txt Assignment\ 5/client_domain")

	os.chdir("Assignment 5/client_domain")
	os.system("./client test_commands.txt " + server_ip)
	# print("\n---------- client existed successfully ----------")


def start_syncheck_testcase(server_ip):
	local_file = "client_domain/Local Directory/test_syncheck.txt"
	remote_file = "server_domain/Remote Directory/test_syncheck.txt"

	syncheck_file = open(local_file, "w")
	syncheck_file.write("abcdefg")
	syncheck_file.close()

	syncheck_file = open(remote_file, "w")
	syncheck_file.write("7777777")
	syncheck_file.close()

	os.chdir(root_dir)
	os.system("cp syncheck_testcase/test_commands.txt Assignment\ 5/client_domain")

	os.chdir("Assignment 5/client_domain")
	os.system("./client test_commands.txt " + server_ip)
	# print("\n---------- client existed successfully ----------")


def start_lock_testcase(server_ip, i):
	if i == 1:
		lock_file = "server_domain/Remote Directory/test_lock.txt"
		syncheck_file = open(lock_file, "w")
		syncheck_file.write("abcde")
		syncheck_file.close()
		os.chdir(root_dir)
		os.system("cp lock_testcase/test_commands1.txt Assignment\ 5/client_domain")
		os.system("cp lock_testcase/test_commands2.txt Assignment\ 5/client_domain")
		os.chdir("Assignment 5/client_domain")
		os.system("./client test_commands1.txt " + server_ip + " &>/dev/null")
		# print("\n---------- client 1 existed successfully ----------")
	if i == 2:
		os.chdir("client_domain")
		os.system("./client test_commands2.txt " + server_ip)
		# print("\n---------- client 2 existed successfully ----------")


def start_multi_client_testcase(server_ip, i):
	if i == 1:
		file_path = "server_domain/Remote Directory/"
		client_file = open(file_path + "file1.txt", "w")
		client_file.write("Text File 1")
		client_file.close()

		client_file = open(file_path + "file2.txt", "w")
		client_file.write("Text File 2")
		client_file.close()

		client_file = open(file_path + "file3.txt", "w")
		client_file.write("Text File 3")
		client_file.close()

		os.chdir(root_dir)
		os.system("cp multiclient_testcase/test_commands1.txt Assignment\ 5/client_domain")
		os.system("cp multiclient_testcase/test_commands2.txt Assignment\ 5/client_domain")
		os.system("cp multiclient_testcase/test_commands3.txt Assignment\ 5/client_domain")

		os.chdir("Assignment 5/client_domain")
		os.system("./client test_commands1.txt " + server_ip)
		# print("\n---------- client 1 existed successfully ----------")

	if i == 2:
		os.chdir("client_domain")
		os.system("./client test_commands2.txt " + server_ip)
		# print("\n---------- client 2 existed successfully ----------")

	if i == 3:
		os.chdir("client_domain")
		os.system("./client test_commands3.txt " + server_ip)
		# print("\n---------- client 3 existed successfully ----------")


if __name__ == "__main__":
	os.system("clear")
	server_ip = "127.0.0.1"
	server_port = 9999

	try:
		server_socket = socket.socket()
		server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		server_socket.bind((server_ip, server_port))
		server_socket.close()
	except OSError:
		server_socket.close()
		sys.exit("\nCannot run the autograder because the IP address " + server_ip + " with port " + str(server_port) + " is occupied or does not exist.\nIf you want to run the autograder on a different IP/port, please edit them inside the python code.")

	if not exists("Assignment 5.zip"):
		sys.exit("Cannot find Assignment 5.zip\nTerminating the autograder")


	os.system("unzip Assignment\ 5")

	if not exists("Assignment 5/auto_compile.py"):
		sys.exit("Cannot find auto_compile.py\nTerminating the autograder")

	root_dir = os.getcwd()
	os.chdir("Assignment 5")
	os.system("python3 auto_compile.py")

	os.system("mv Client\ Domain client_domain")
	os.system("mv Server\ Domain server_domain")

	########### Download Testcase Code ##########

	print("\n---------- starting download testcase ----------")
	# print("-----starting server process-----")
	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	# print("-----starting client process-----")
	client_process = Process(target=start_download_testcase, args=(server_ip, ))
	client_process.start()
	client_pid = client_process.pid

	time.sleep(0.5)  # It should be instantaneous but we will give it 0.5 second
	terminate_processes(client_pid, server_pid)  # For hanging processes.

	print("\n\n########## Download Testcase Results ##########\n")
	can_download = check_hash("file1.txt")
	check_hash("file2.jpeg")

	########### Upload Testcase Code ##########

	print("\n\n\n---------- starting upload testcase ----------")
	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client_process = Process(target=start_upload_testcase, args=(server_ip, ))
	client_process.start()
	client_pid = client_process.pid

	time.sleep(0.5)
	terminate_processes(client_pid, server_pid)

	print("\n\n########## Upload Testcase Results ##########\n")
	check_hash("file1.txt")
	check_hash("file2.jpeg")

	########### Append Testcase Code ##########

	print("\n\n\n---------- starting append testcase ----------")
	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client_process = Process(target=start_append_testcase, args=(server_ip, ))
	client_process.start()
	client_pid = client_process.pid

	time.sleep(0.5)
	terminate_processes(client_pid, server_pid)

	print("\n\n########## Append Testcase Results ##########\n")

	expected_short_test_file = "starting line\nsingle_string"
	short_test_file = open("server_domain/Remote Directory/file1.txt")
	# print(short_test_file.read())
	if expected_short_test_file in short_test_file.read():
		print("SUCCESS: file1.txt content match")
	else:
		print("FAILURE: file1.txt content does not match")
	short_test_file.close()

	expected_long_test_file = "sentence test line 1\nsentence test line 2\nsentence test line 3"
	long_test_file = open("server_domain/Remote Directory/file2.txt")
	# print(long_test_file.read())
	if expected_long_test_file in long_test_file.read():
		print("SUCCESS: file2.txt content match")
	else:
		print("FAILURE: file2.txt content does not match")
	long_test_file.close()

	########### Delete Testcase Code ##########

	print("\n\n\n---------- starting delete testcase ----------")
	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client_process = Process(target=start_delete_testcase, args=(server_ip, ))
	client_process.start()
	client_pid = client_process.pid

	time.sleep(0.5)
	terminate_processes(client_pid, server_pid)

	print("\n\n########## Delete Testcase Results ##########\n")

	try:
		file_path = "server_domain/Remote Directory/"
		file_name = "file1.txt"
		deleted_file = open(file_path + file_name, 'r')
		deleted_file.close()
		print("FAILURE: file", file_name, "was not deleted")
	except FileNotFoundError:
		print("SUCCESS: file", file_name, "was deleted successfully")

	try:
		file_path = "server_domain/Remote Directory/"
		file_name = "file3.jpeg"
		deleted_file = open(file_path + file_name, 'r')
		deleted_file.close()
	except FileNotFoundError:
		print("FAILURE: file", file_name, "should not be deleted from Remote Directory")

	try:
		file_path = "client_domain/Local Directory/"
		file_name = "file2.txt"
		deleted_file = open(file_path + file_name, 'r')
		deleted_file.close()
	except FileNotFoundError:
		print("FAILURE: file", file_name, "should not be deleted from Local Directory")

	########### Syncheck Testcase Code ##########

	print("\n\n\n---------- starting syncheck testcase ----------")

	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client_process = Process(target=start_syncheck_testcase, args=(server_ip, ))
	client_process.start()
	client_pid = client_process.pid

	time.sleep(0.5)
	terminate_processes(client_pid, server_pid)

	print("\n\n########## Syncheck Testcase Results ##########\n")

	print("expected output\n")
	print("Welcome to ICS53 Online Cloud Storage.")
	print("> syncheck test_syncheck.txt")
	print("Sync Check Report:")
	print("- Local Directory:")
	print("-- File Size: 7 bytes.")
	print("- Remote Directory:")
	print("-- File Size: 7 bytes.")
	print("-- Sync Status: unsynced.")
	print("-- Lock Status: unlocked.")

	########### Lock Testcase Code ##########

	print("\n\n\n---------- starting lock testcase ----------")

	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client1_process = Process(target=start_lock_testcase, args=(server_ip, 1,))
	client1_process.start()
	client1_pid = client1_process.pid

	time.sleep(1)

	client2_process = Process(target=start_lock_testcase, args=(server_ip, 2,))
	client2_process.start()
	client2_pid = client2_process.pid

	time.sleep(2)

	terminate_processes(client1_pid, server_pid)
	terminate_processes(client2_pid, server_pid)

	print("\n\n########## Lock Testcase Results ##########\n")

	try:
		file_path = "client_domain/Local Directory/"
		file_name = "test_lock.txt"
		deleted_file = open(file_path + file_name, 'r')
		deleted_file.close()
		print("FAILURE: file", file_name, "was not locked\n")
	except FileNotFoundError:
		if not can_download:
			print("FAILURE: cannot perform a download operation\n")
		else:
			print("SUCCESS: file", file_name, "was locked successfully\n")


	########### Multi-Client Server Testcase Code ##########

	print("\n\n\n---------- starting multi-client testcase ----------")

	server_process = Process(target=start_server, args=(server_ip, ))
	server_process.start()
	server_pid = server_process.pid
	time.sleep(0.5)

	client1_process = Process(target=start_multi_client_testcase, args=(server_ip, 1,))
	client1_process.start()
	client1_pid = client1_process.pid

	time.sleep(0.2)

	client2_process = Process(target=start_multi_client_testcase, args=(server_ip, 2,))
	client2_process.start()
	client2_pid = client2_process.pid

	time.sleep(0.2)

	client3_process = Process(target=start_multi_client_testcase, args=(server_ip, 3,))
	client3_process.start()
	client3_pid = client3_process.pid

	time.sleep(2)

	terminate_processes(client1_pid, client2_pid)
	terminate_processes(client3_pid, server_pid)

	print("\n\n########## Multi-Client Testcase Results ##########\n")

	try:
		os.system("chmod +rwx client_domain/Local\ Directory/file1.txt &>/dev/null")
		os.system("chmod +rwx client_domain/Local\ Directory/file2.txt &>/dev/null")
		os.system("chmod +rwx client_domain/Local\ Directory/file3.txt &>/dev/null")
		file_path = "client_domain/Local Directory/"
		downloaded_file = open(file_path + "file1.txt", 'r')
		downloaded_file.close()
		downloaded_file = open(file_path + "file2.txt", 'r')
		downloaded_file.close()
		downloaded_file = open(file_path + "file3.txt", 'r')
		downloaded_file.close()
		print("SUCCESS: all clients connected successfully")
	except FileNotFoundError:
		print("FAILURE: some clients were not connected")

	print("\n\n################################# Autograder Has Concluded #################################\n\n")
