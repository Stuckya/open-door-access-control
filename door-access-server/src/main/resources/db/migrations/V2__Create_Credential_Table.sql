CREATE EXTENSION IF NOT EXISTS "uuid-ossp" WITH SCHEMA public;

CREATE TABLE user (
                     id UUID NOT NULL DEFAULT public.uuid_generate_v4(),
                     created_on TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
                     created_by UUID NOT NULL,
                     last_modified_on TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
                     last_modified_by UUID,
                     username TEXT NOT NULL,
                     email TEXT NOT NULL,
                     CONSTRAINT pk_user PRIMARY KEY (id),
);
