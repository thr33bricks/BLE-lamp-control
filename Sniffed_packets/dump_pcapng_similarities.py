#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri May  5 01:46:42 2023

@author: rossi
"""

import sys
import logging
import os
import copy
from typing import List

from pcapng import FileScanner
from pcapng.blocks import EnhancedPacket
from scapy.layers import bluetooth4LE


# -----------------------------------------------------------------------------
class BLE_Payload_Extractor:

    # Default value is for nRF52840 generated pcapng
    BLE_PACKET_START = 17  
    ADVERTISING_START = BLE_PACKET_START + 12
    
    # -------------------------------------------------------------------------
    def __init__(self, folder: str):
        self.log = logging.getLogger(__name__)
        if not os.path.isdir(folder):
            err = f"Unexisting folder provided: {folder}"
            self.log.error(err)
            raise IOError(err)
        self.folder = folder

    # -------------------------------------------------------------------------
    def dump_similarities_in_all_pcapng(self):
        for bfn in os.listdir(self.folder):
            if bfn.endswith(".pcapng"):
                print('-'*(31*3-1))
                print(f'File: {bfn}')
                ffn = os.path.join(self.folder, bfn)
                adv_data_list = self.get_advert_data(ffn)
                for i in self.get_similarities(adv_data_list):
                    print(i)
                
    # -------------------------------------------------------------------------
    def get_advert_data(self, ffn: str) -> List[bytes]:
        """
        Extract the BLE payload (data) from each package in a pcapng file.
    
        Parameters
        ----------
        ffn : str
            The pcapng full file name to read the payload from. It must be 
            (obtained by nordic nRF dongle). 
    
        Returns
        -------
        _arr : List[bytes]
            List of all payloads inside the pcapng file.
    
        """
        _arr = []
        with open(ffn, "rb") as fp:
            scanner = FileScanner(fp)
            for block in scanner:
                if isinstance(block, EnhancedPacket):
                    d = block.packet_data
                    # decoded = bluetooth4LE.BTLE(d[self.NRF_BLE_PACKET_START:])
                    AdvData = d[self.ADVERTISING_START:self.ADVERTISING_START+31]
                    # _arr.append(AdvData.hex(' ', 1))
                    _arr.append(AdvData)
        return _arr

    # -------------------------------------------------------------------------
    def get_similarities(self, adv_data_list: List[bytes]) -> List[str]:
        t_adv_data_list = [i.hex(' ') for i in adv_data_list]
        
        # Find distinct protocols by looking at the first byte (i.e. 2 HEX)
        distinct_prot_prefs = []
        ind = 0
        for adv in t_adv_data_list:
            if adv[0:18] not in distinct_prot_prefs:
                # self.log.debug(f"New prefix found in {adv}")
                # print(f"New prefix found at line {ind} in {adv}")
                distinct_prot_prefs.append(adv[0:18])
                ind += 1
        
        res = []
        # Find similarities in all distinct protocols
        for pref in distinct_prot_prefs:
            filt_arr = [i for i in t_adv_data_list if i.startswith(pref)]
            distinct_list = list(filt_arr[0])
            for packet in filt_arr:
                # Check if each char repeats in the next line
                for i, ch in enumerate(packet):
                    if distinct_list[i] != ch:
                        distinct_list[i] = '-'
            dist = "".join([str(i) for i in distinct_list])
            res.append(dist)
        
        self.adv_data_list = t_adv_data_list
        return res
        
        
# ----------------------------------------------------------------------------- 
if __name__ == '__main__':

    logging.basicConfig()
    # PCAP_DIR = 'LampD/remote/raw_pcap/'
    # PCAP_DIR = 'LampR/remote/raw_pcap/'
    
    # PCAP_DIR = 'LampD/Hi_Light_PRO__com.cxw.hilightpro/raw_pcap/'
    # PCAP_DIR = 'LampD/LampSmartPro__com.jingyuan.lamp/raw_pcap/'
    PCAP_DIR = 'LampD/SmartLightControl__com.hf.ble_light/raw_pcap/'
    logging.basicConfig()
    analyzator = BLE_Payload_Extractor(PCAP_DIR)
    analyzator.dump_similarities_in_all_pcapng()

