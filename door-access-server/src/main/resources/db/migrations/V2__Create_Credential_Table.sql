CREATE TYPE credential_status AS ENUM ('ACTIVE', 'INACTIVE');

CREATE TABLE credential (
                     id UUID NOT NULL DEFAULT public.uuid_generate_v4(),
                     created_on TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
                     created_by UUID NOT NULL,
                     last_modified_on TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
                     last_modified_by UUID,
                     name TEXT NULLABLE,
                     last_used TIMESTAMP WITH TIME ZONE NULLABLE,
                     status credential_status NOT NULL,
                     CONSTRAINT pk_credential PRIMARY KEY (id),
);

CREATE TABLE wiegand_credential (
                    code integer NOT NULL
);
