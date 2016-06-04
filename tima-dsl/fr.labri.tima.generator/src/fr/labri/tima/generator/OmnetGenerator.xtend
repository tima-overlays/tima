package fr.labri.tima.generator

import fr.labri.tima.ir.IRAutomata
import org.eclipse.xtext.generator.IFileSystemAccess2
import org.eclipse.xtext.generator.IGeneratorContext

class OmnetGenerator extends NativeGenerator {
	
	new(IRAutomata a) {
		super(a)
	}
	
	override def void generateFiles(String name, IFileSystemAccess2 fsa, IGeneratorContext context) {
		super.generateFiles(name, fsa, context)
		
		fsa.generateFile('''«name»/«name»_inet.ned''', omnet_inet_app_descriptor(name, name + "_inet"))
		fsa.generateFile('''«name»/«name»_inet.h''', omnet_inet_app_header(name + "_inet"))
		fsa.generateFile('''«name»/«name»_inet.cc''', 
			omnet_inet_app_code(name + "_inet", name)
		)
	}
	
	def omnet_inet_app_descriptor(String package_name, String project_name){
	'''
	package inet.applications.«package_name»;
	
	import inet.applications.contract.IUDPApp;
	
	import inet.applications.tima.TimaAppBase;
	
	//
	// This module is an application that implement the Gossip protocol
	// to disemminate messages.
	//
	// Some nodes distribute messages in the network using unicast UDP.
	//
	simple «project_name» extends TimaAppBase like IUDPApp
	{
	    parameters:
	        @class(«project_name»);
	}
	'''
	}
	
	def String omnet_inet_app_code(String project_name, String protocol_name) {
	'''
	#include "«project_name».h"
	
	namespace «protocol_name» {
	
	uint32_t get_nr_automaton_in_«protocol_name»();	
	struct tima::Automaton& get_automaton_«protocol_name»(uint32_t idx);
	
	void
	init_device_data_«protocol_name»(
		std::string& device_name,
		std::map<std::string, std::string>& options,
		std::shared_ptr<tima::GlobalStorage> st);
	
	}
	
	namespace inet {
	
	Define_Module(«project_name»);
	
	std::vector<tima::Automaton*>
	«project_name»::build_stl_version()
	{
	  std::vector<tima::Automaton*> automatas;
	  uint32_t n = «protocol_name»::get_nr_automaton_in_«protocol_name»();
	  for (size_t i = 0; i < n; i++) {
	    /* code */
	    struct tima::Automaton* x = &«protocol_name»::get_automaton_«protocol_name»(i);
	    automatas.push_back(x);
	  }
	  return automatas;
	}
	
	tima::device_initialization_t
	«project_name»::get_device_initialization_routine()
	{
	    return &«protocol_name»::init_device_data_«protocol_name»;
	}
	
	}
	'''
	}
	
	def omnet_inet_app_header(String project_name) {
	'''
	#ifndef __INET_«project_name.toUpperCase»_H_
	#define __INET_«project_name.toUpperCase»_H_
	
	#include <omnetpp.h>
	
	#include <map>
	#include <vector>
	#include <string>
	#include <memory>
	
	#include "inet/applications/tima/executor.h"
	#include "inet/applications/tima/automata.h"
	#include "inet/applications/tima/mailbox.h"
	#include "inet/applications/tima/tima.h"
	
	#include "inet/applications/tima/omnetpp_tima_nature.h"
	
	namespace inet {
	
	
	class INET_API «project_name» : public tima::omnet::TimaAppBase
	{
	  private:
	  
	  	virtual std::vector<tima::Automaton*> build_stl_version() override;
	  	virtual tima::device_initialization_t get_device_initialization_routine() override;
	};
	
	} //namespace
	
	#endif
	'''
	}
}
