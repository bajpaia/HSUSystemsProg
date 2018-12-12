import multiprocessing as mp

class SharingIsCaring(object):
	def __init__(self,name):
		self.name=name
	
	def saySomething(self):
		proc_name= mp.current_process().name
		print ('Giving my cake to process no %s because %s taught me well' %(proc_name,self.name))
	
def worker(q):
	obj= q.get()
	obj.saySomething()

if __name__=='__main__':

	q = mp.Queue()
	proc = mp.Process(target=worker,args=(q,))
	proc.start()
	q.put(SharingIsCaring('Mom'))
	q.close()
	q.join_thread()
	proc.join
