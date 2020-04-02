
import axios from 'axios'
const REST_END_POINT = "https://europe-west1-iothomework-b9a5b.cloudfunctions.net";

export class PersistanceStorage {

    constructor() {

        if (!!PersistanceStorage.instance) {
            return PersistanceStorage.instance;
        }

        PersistanceStorage.instance = this;
        return this;
    }

    /**
     * Get all the logs in the database
     * @author Giulio Serra <serra.1904089@studenti.uniroma1.it>
     */
    getLogs(){
        return new Promise((res, rej) => {

            const URL = REST_END_POINT + "/getLogs";
            axios.get(URL).then(response => {
                return res(response)
            }).catch(error=>{
                return rej(error);
            })
        })
    }
}