
# Behnam Dezfouli
# CSEN, Santa Clara University

# This program implements a simple web server that serves html and jpg files

# Input arguments:
# argv[1]: Sever's port number


from socket import *  # Import socket module
import sys            # To terminate the program


if __name__ == "__main__":

    # check if port number is provided
    if len(sys.argv) != 2:
        print ('Usage: python %s <PortNumber>' % (sys.argv[0]))
        sys.exit()
    
    
    # STUDENT WORK
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    serverPort = int(sys.argv[1])
    serverSocket.bind(("",serverPort))
    
    # Server should be up and running and listening to the incoming connections
    while True:
        print('The server is ready to respond to incoming requests...')
	serverSocket.listen(1)
	connectionSocket, addr = serverSocket.accept()
	message = connectionSocket.recv(1024)
    


        # STUDENT WORK
        file_name = message.split(' ')[1]
        file_name = file_name[1:]
	if (len(file_name.split('.')[1]) < 2){
	    raise IOError;
	}
	file_extension = file_name.split('.')[1];
	

        
        try:

            # STUDENT WORK
            response_head = 'HTTP/1.1 200 OK\n' 
            response_head += 'Server: SCU COEN Python Web Server\n'

	    try:
	    	f = open(file_name, 'rb')
            	outputdata = f.read()
            	f.close()
	    except Exception:
		f = open('asdf.html', 'rb')
		outputdata = f.read()
		f.close()
		file_extension = 'html'



            if (file_extension == 'html'):
                response_headers = { 'Content-Type': 'text/html; encoding=utf8\n'}
            elif (file_extension == 'jpg'):
                response_headers = { 'Content-Type': 'image/jpeg; encoding=utf8\n'}
            else:
                print ('Invalid file type, we only support html and jpg!')
                continue

	    


	    # STUDENT WORK
            response_head += 'Content-Type: '
	    response_head += str(response_headers['Content-Type'])
            response_head += 'Content_length: ' + str(len(outputdata))          
	    response_head += '\n\n'
 	    print(response_head)
	    connectionSocket.send(response_head)
	    connectionSocket.send(outputdata)
	    connectionSocket.shutdown(SHUT_RDWR)
	    connectionSocket.close()
            


            
            
        #Raised when an I/O operation (such as a print statement, the built-in open() function or a method of a file object) fails for an I/O-related reason, e.g., "file not found" or "disk full"
        except IOError:
            # STUDENT WORK
            connectionSocket.shutdown(SHUT_RDWR)
	    connectionSocket.close()
	    serverSocket.close()


# STUDENT WORK
